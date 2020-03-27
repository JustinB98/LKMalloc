#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "lkmalloc.h"

static void assert_bytes(char *buf, int byte, u_int len) {
	for (u_int i = 0; i < len; ++i) {
		if (buf[i] != byte) exit(EXIT_FAILURE);
	}
}

/* size, malloc_flags, offset, byte, len */
int main(int argc, char *argv[]) {
	if (argc != 6) return EXIT_FAILURE;
	u_int size = atoi(argv[1]);
	u_int malloc_flags;
	if (sscanf(argv[2], "%i", &malloc_flags) < 0) return EXIT_FAILURE;
	int offset = atoi(argv[3]);
	int byte;
	if (sscanf(argv[4], "%i", &byte) < 0) return EXIT_FAILURE;
	u_int len = atoi(argv[5]);
	void *buf = NULL;
	int ret = lkmalloc(size, &buf, malloc_flags);
	if (ret < 0) return EXIT_FAILURE;
	assert_bytes(buf + offset, byte, len);
	ret = lkfree(&buf, LKF_REG);
	if (ret < 0) return EXIT_FAILURE;
	ret = lkreport(STDERR_FILENO, LKR_MATCH);
	return ret == 2 ? EXIT_SUCCESS : EXIT_FAILURE;
}
