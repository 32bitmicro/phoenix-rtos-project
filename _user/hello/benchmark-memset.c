#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define ITERATIONS 10000

#define OFFSETS 2

char src[32768 + 16];
char dst[32768 + 16];

struct {
	time_t bytes8[OFFSETS];
	time_t bytes16[OFFSETS];
	time_t bytes32[OFFSETS];
	time_t bytes64[OFFSETS];
	time_t bytes128[OFFSETS];
	time_t bytes256[OFFSETS];
	time_t bytes512[OFFSETS];
	time_t bytes1024[OFFSETS];
	time_t bytes2048[OFFSETS];
	time_t bytes4096[OFFSETS];
	time_t bytes8192[OFFSETS];
	time_t bytes16384[OFFSETS];
	time_t bytes32768[OFFSETS];
} times;



void *old_memset(void *dst, int v, size_t l)
{
	__asm__ volatile
	(" \
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
	: "r" (dst), "r" (v & 0xff), "r" (l)
	: "r1", "r2", "r3", "r4", "memory", "cc");

	return dst;
}


void summary(void)
{
	printf("off   ");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8d | ", i);
	}
	printf("\n");
	printf("----------------------------------------------\n");
	printf("b8:   ");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes8[i]);
	}
	printf("\n");
	printf("b16:  ");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes16[i]);
	}
	printf("\n");
	printf("b32:  ");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes32[i]);
	}
	printf("\n");
	printf("b64:  ");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes64[i]);
	}
	printf("\n");
	printf("b128: ");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes128[i]);
	}
	printf("\n");
	printf("b256: ");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes256[i]);
	}
	printf("\n");
	printf("b512: ");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes512[i]);
	}
	printf("\n");
	printf("b1024:");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes1024[i]);
	}
	printf("\n");
	printf("b2048:");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes2048[i]);
	}
	printf("\n");
	printf("b4096:");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes4096[i]);
	}
	printf("\n");
	printf("b8192:");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes8192[i]);
	}
	printf("\n");
	printf("b16384");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes16384[i]);
	}
	printf("\n");
	printf("b32768");
	for (int i = 0; i < OFFSETS; i++) {
		printf("%8lld | ", times.bytes32768[i]);
	}
	printf("\n");
}

time_t benchmark(int size, int offset)
{
	time_t start, end;

	gettime(&start, NULL);
	for (int i = 0; i < ITERATIONS; i++) {
		memset(dst + offset, 0, size);
	}
	gettime(&end, NULL);

	return (end - start);
}


time_t benchmarkUnalignDst(int size, int offset)
{
	time_t start, end;

	gettime(&start, NULL);
	for (int i = 0; i < ITERATIONS; i++) {
		memset(dst + offset + 1, 0, size);
	}
	gettime(&end, NULL);

	return (end - start);
}


time_t benchmarkold(int size, int offset)
{
	time_t start, end;

	gettime(&start, NULL);
	for (int i = 0; i < ITERATIONS; i++) {
		old_memset(dst + offset, 0, size);
	}
	gettime(&end, NULL);

	return (end - start);
}

int main(void)
{
	for (int i = 0; i < sizeof(src); i++) {
		src[i] = i % 256;
	}

	for (int offset = 0; offset < OFFSETS; offset++) {
		times.bytes8[offset] = benchmark(8, offset);
		times.bytes16[offset] = benchmark(16, offset);
		times.bytes32[offset] = benchmark(32, offset);
		times.bytes64[offset] = benchmark(64, offset);
		times.bytes128[offset] = benchmark(128, offset);
		times.bytes256[offset] = benchmark(256, offset);
		times.bytes512[offset] = benchmark(512, offset);
		times.bytes1024[offset] = benchmark(1024, offset);
		times.bytes2048[offset] = benchmark(2048, offset);
		times.bytes4096[offset] = benchmark(4096, offset);
		times.bytes8192[offset] = benchmark(8192, offset);
		times.bytes16384[offset] = benchmark(16384, offset);
		times.bytes32768[offset] = benchmark(32768, offset);
	}

	printf("memset\n");
	summary();

	for (int offset = 0; offset < OFFSETS; offset++) {
		times.bytes8[offset] = benchmarkUnalignDst(8, offset);
		times.bytes16[offset] = benchmarkUnalignDst(16, offset);
		times.bytes32[offset] = benchmarkUnalignDst(32, offset);
		times.bytes64[offset] = benchmarkUnalignDst(64, offset);
		times.bytes128[offset] = benchmarkUnalignDst(128, offset);
		times.bytes256[offset] = benchmarkUnalignDst(256, offset);
		times.bytes512[offset] = benchmarkUnalignDst(560, offset);
		times.bytes1024[offset] = benchmarkUnalignDst(1024, offset);
		times.bytes2048[offset] = benchmarkUnalignDst(2048, offset);
		times.bytes4096[offset] = benchmarkUnalignDst(4096, offset);
		times.bytes8192[offset] = benchmarkUnalignDst(8192, offset);
		times.bytes16384[offset] = benchmarkUnalignDst(16384, offset);
		times.bytes32768[offset] = benchmarkUnalignDst(32768, offset);
	}

	printf("memset unaligned dst\n");
	summary();

	for (int offset = 0; offset < OFFSETS; offset++) {
		times.bytes8[offset] = benchmarkold(8, offset);
		times.bytes16[offset] = benchmarkold(16, offset);
		times.bytes32[offset] = benchmarkold(32, offset);
		times.bytes64[offset] = benchmarkold(64, offset);
		times.bytes128[offset] = benchmarkold(128, offset);
		times.bytes256[offset] = benchmarkold(256, offset);
		times.bytes512[offset] = benchmarkold(512, offset);
		times.bytes1024[offset] = benchmarkold(1024, offset);
		times.bytes2048[offset] = benchmarkold(2048, offset);
		times.bytes4096[offset] = benchmarkold(4096, offset);
		times.bytes8192[offset] = benchmarkold(8192, offset);
		times.bytes16384[offset] = benchmarkold(16384, offset);
		times.bytes32768[offset] = benchmarkold(32768, offset);
	}

	printf("old memset\n");
	summary();

	return 0;
}