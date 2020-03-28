#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "lkmalloc.h"
#include "test_utils.h"

static void assert_bytes(char *buf, int byte, u_int len) {
	for (u_int i = 0; i < len; ++i) {
		int c = buf[i];
		if (c != byte) exit(EXIT_FAILURE);
	}
}

/* bin/assert_memory_block size, malloc_flags, offset, byte, len */
int main(int argc, char *argv[]) {
	if (argc != 6) return EXIT_FAILURE;
	u_int size, malloc_flags, len;
	int offset, byte;
	CONVERT_NUM(argv[1], &size);
	CONVERT_NUM(argv[2], &malloc_flags);
	CONVERT_NUM(argv[3], &offset);
	CONVERT_NUM(argv[4], &byte);
	CONVERT_NUM(argv[5], &len);
	void *buf = NULL;
	int ret = lkmalloc(size, &buf, malloc_flags);
	if (ret < 0) return EXIT_FAILURE;
	assert_bytes(buf + offset, byte, len);
	ret = lkfree(&buf, LKF_REG);
	if (ret < 0) return EXIT_FAILURE;
	ret = lkreport(STDERR_FILENO, LKR_MATCH);
	return ret == 2 ? EXIT_SUCCESS : EXIT_FAILURE;
}
