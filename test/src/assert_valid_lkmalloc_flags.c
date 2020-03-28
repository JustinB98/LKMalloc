#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "lkmalloc.h"
#include "test_utils.h"

/* assert_valid_lkmalloc_flags size malloc_flags assert_success (0 will assert_fail) */
int main(int argc, char *argv[]) {
	if (argc != 4) return EXIT_FAILURE;
	u_int malloc_flags, size;
	int assert_success;
	CONVERT_NUM(argv[1], &size);
	CONVERT_NUM(argv[2], &malloc_flags);
	CONVERT_NUM(argv[3], &assert_success);
	void *buf = NULL;
	int ret = lkmalloc(size, &buf, malloc_flags);
	if (assert_success) {
		return ret != -EINVAL ? EXIT_SUCCESS : EXIT_FAILURE;
	} else {
		return ret == -EINVAL ? EXIT_SUCCESS : EXIT_FAILURE;
	}
}
