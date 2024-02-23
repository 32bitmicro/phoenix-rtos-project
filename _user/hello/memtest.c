#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <limits.h>

void *oldmemcpy(void *dst, const void *src, size_t l)
{
	__asm__ volatile(" \
		mov r1, %2; \
		mov r3, %1; \
		mov r4, %0; \
		orr r2, r3, r4; \
		ands r2, #3; \
		bne 2f; \
	1: \
		cmp r1, #4; \
		ittt hs; \
		ldrhs r2, [r3], #4; \
		strhs r2, [r4], #4; \
		subshs r1, #4; \
		bhs 1b; \
	2: \
		cmp r1, #0; \
		ittt ne; \
		ldrbne r2, [r3], #1; \
		strbne r2, [r4], #1; \
		subsne r1, #1; \
		bne 2b"
					 :
					 : "r"(dst), "r"(src), "r"(l)
					 : "r1", "r2", "r3", "r4", "memory", "cc");

	return dst;
}


void *oldmemset(void *dst, int v, size_t l)
{
	__asm__ volatile(" \
		mov r1, %2; \
		mov r3, %1; \
		orr r3, r3, r3, lsl #8; \
		orr r3, r3, r3, lsl #16; \
		mov r4, %0; \
		ands r2, r4, #3; \
		bne 2f; \
	1: \
		cmp r1, #4; \
		itt hs; \
		strhs r3, [r4], #4; \
		subshs r1, #4; \
		bhs 1b; \
	2: \
		cmp r1, #0; \
		itt ne; \
		strbne r3, [r4], #1; \
		subsne r1, #1; \
		bne 2b"
					 :
					 : "r"(dst), "r"(v & 0xff), "r"(l)
					 : "r1", "r2", "r3", "r4", "memory", "cc");

	return dst;
}


int main(void)
{
	const size_t max_copy_size = 1024;

	unsigned char *source = (unsigned char *)mmap(NULL, 3 * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	unsigned char *destination = (unsigned char *)mmap(NULL, 3 * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (source == MAP_FAILED || destination == MAP_FAILED) {
		printf("mmap fail\n");
		return 1;
	}

	srand(0);
	for (size_t copy_size = 0; copy_size < max_copy_size; copy_size++) {
		for (size_t srcOff = 0; srcOff < 16; srcOff++) {
			for (size_t dstOff = 0; dstOff < 16; dstOff++) {
				for (size_t i = 0; i < 3 * PAGE_SIZE; i++) {
					char v = rand() % 256;
					source[i] = v == 0x55 ? 0x56 : v;
				}
				oldmemset(destination, 0x55, 3 * PAGE_SIZE);
				printf("copy_size: %zu, srcOff: %zu, dstOff: %zu\n", copy_size, srcOff, dstOff);
				memcpy(destination + dstOff, source + srcOff, copy_size);

				if (memcmp(destination + dstOff, source + srcOff, copy_size) != 0) {
					printf("Copy failed\n", copy_size, srcOff, dstOff);
					return 1;
				}
				for (size_t i = 0; i < 3 * PAGE_SIZE; ++i) {
					if (i < dstOff || i >= dstOff + copy_size) {
						if (destination[i] != 0x55) {
							printf("Modified outside of copy region: dest[%zu] = %02x\n", i, destination[i]);
							return 1;
						}
					}
				}
			}
		}
	}

	for (size_t s = 0; s < max_copy_size; s++) {
		printf("memset size: %zu\n", s);
		oldmemset(source, 0xaa, 3 * PAGE_SIZE);
		memset(source, 0x55, s);
		for (size_t i = 0; i < 3 * PAGE_SIZE; ++i) {
			if (i >= s) {
				if (source[i] != 0xaa) {
					printf("Modified outside of memset region: source[%zu] = %02x\n", i, source[i]);
					return 1;
				}
			}
			else {
				if (source[i] != 0x55) {
					printf("Failed to set source[%zu] = %02x\n", i, source[i]);
					return 1;
				}
			}
		}
	}

	munmap(source, 3 * PAGE_SIZE);
	munmap(destination, 3 * PAGE_SIZE);

	return 0;
}