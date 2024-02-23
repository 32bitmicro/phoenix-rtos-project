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


#define UNALIGNED(X, Y) (((long)X & (sizeof(long) - 1)) | ((long)Y & (sizeof(long) - 1)))
#define BIGBLOCKSIZE    (sizeof(long) << 2)
#define LITTLEBLOCKSIZE (sizeof(long))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

void *c_memcpy(void *__restrict dst0, const void *__restrict src0, size_t len0)
{
	char *dst = dst0;
	const char *src = src0;
	long *aligned_dst;
	const long *aligned_src;

	/* If the size is small, or either SRC or DST is unaligned,
	   then punt into the byte copy loop.  This should be rare.  */
	if (!TOO_SMALL(len0) && !UNALIGNED(src, dst)) {
		aligned_dst = (long *)dst;
		aligned_src = (long *)src;

		/* Copy 4X long words at a time if possible.  */
		while (len0 >= BIGBLOCKSIZE) {
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			len0 -= BIGBLOCKSIZE;
		}

		/* Copy one long word at a time if possible.  */
		while (len0 >= LITTLEBLOCKSIZE) {
			*aligned_dst++ = *aligned_src++;
			len0 -= LITTLEBLOCKSIZE;
		}

		/* Pick up any residual with a byte copier.  */
		dst = (char *)aligned_dst;
		src = (char *)aligned_src;
	}

	while (len0--)
		*dst++ = *src++;

	return dst0;
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
		memcpy(dst + offset, src + offset, size);
	}
	gettime(&end, NULL);

	return (end - start);
}


time_t benchmarkUnalignDst(int size, int offset)
{
	time_t start, end;

	gettime(&start, NULL);
	for (int i = 0; i < ITERATIONS; i++) {
		memcpy(dst + offset + 1, src + offset, size);
	}
	gettime(&end, NULL);

	return (end - start);
}


time_t benchmarkUnalignSrc(int size, int offset)
{
	time_t start, end;

	gettime(&start, NULL);
	for (int i = 0; i < ITERATIONS; i++) {
		memcpy(dst + offset, src + offset + 1, size);
	}
	gettime(&end, NULL);

	return (end - start);
}


time_t benchmarkc(int size, int offset)
{
	return 0;
	// time_t start, end;

	// gettime(&start, NULL);
	// for (int i = 0; i < ITERATIONS; i++) {
	// 	c_memcpy(dst + offset, src + offset, size);
	// }
	// gettime(&end, NULL);

	// return (end - start);
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

	printf("memcpy\n");
	summary();

	for (int offset = 0; offset < OFFSETS; offset++) {
		times.bytes8[offset] = benchmarkUnalignDst(8, offset);
		times.bytes16[offset] = benchmarkUnalignDst(16, offset);
		times.bytes32[offset] = benchmarkUnalignDst(32, offset);
		times.bytes64[offset] = benchmarkUnalignDst(64, offset);
		times.bytes128[offset] = benchmarkUnalignDst(128, offset);
		times.bytes256[offset] = benchmarkUnalignDst(256, offset);
		times.bytes512[offset] = benchmarkUnalignDst(512, offset);
		times.bytes1024[offset] = benchmarkUnalignDst(1024, offset);
		times.bytes2048[offset] = benchmarkUnalignDst(2048, offset);
		times.bytes4096[offset] = benchmarkUnalignDst(4096, offset);
		times.bytes8192[offset] = benchmarkUnalignDst(8192, offset);
		times.bytes16384[offset] = benchmarkUnalignDst(16384, offset);
		times.bytes32768[offset] = benchmarkUnalignDst(32768, offset);
	}

	printf("memcpy unaligned dst\n");
	summary();


	for (int offset = 0; offset < OFFSETS; offset++) {
		times.bytes8[offset] = benchmarkUnalignSrc(8, offset);
		times.bytes16[offset] = benchmarkUnalignSrc(16, offset);
		times.bytes32[offset] = benchmarkUnalignSrc(32, offset);
		times.bytes64[offset] = benchmarkUnalignSrc(64, offset);
		times.bytes128[offset] = benchmarkUnalignSrc(128, offset);
		times.bytes256[offset] = benchmarkUnalignSrc(256, offset);
		times.bytes512[offset] = benchmarkUnalignSrc(512, offset);
		times.bytes1024[offset] = benchmarkUnalignSrc(1024, offset);
		times.bytes2048[offset] = benchmarkUnalignSrc(2048, offset);
		times.bytes4096[offset] = benchmarkUnalignSrc(4096, offset);
		times.bytes8192[offset] = benchmarkUnalignSrc(8192, offset);
		times.bytes16384[offset] = benchmarkUnalignSrc(16384, offset);
		times.bytes32768[offset] = benchmarkUnalignSrc(32768, offset);
	}

	printf("memcpy unaligned src\n");
	summary();


	for (int offset = 0; offset < OFFSETS; offset++) {
		times.bytes8[offset] = benchmarkc(8, offset);
		times.bytes16[offset] = benchmarkc(16, offset);
		times.bytes32[offset] = benchmarkc(32, offset);
		times.bytes64[offset] = benchmarkc(64, offset);
		times.bytes128[offset] = benchmarkc(128, offset);
		times.bytes256[offset] = benchmarkc(256, offset);
		times.bytes512[offset] = benchmarkc(512, offset);
		times.bytes1024[offset] = benchmarkc(1024, offset);
		times.bytes2048[offset] = benchmarkc(2048, offset);
		times.bytes4096[offset] = benchmarkc(4096, offset);
		times.bytes8192[offset] = benchmarkc(8192, offset);
		times.bytes16384[offset] = benchmarkc(16384, offset);
		times.bytes32768[offset] = benchmarkc(32768, offset);
	}

	printf("c_memcpy\n");
	summary();

	return 0;
}