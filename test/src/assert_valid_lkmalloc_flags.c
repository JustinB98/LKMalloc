#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "lkmalloc.h"

/* assert_valid_lkmalloc_flags malloc_flags assert_success (0 will assert_fail) */
int main(int argc, char *argv[]) {
	if (argc != 3) return EXIT_FAILURE;
	u_int malloc_flags;
	int assert_success;
	int ret = sscanf(argv[1], "%i", &malloc_flags);
	assert_success = atoi(argv[2]);
	if (ret < 0) return EXIT_FAILURE;
	void *buf = NULL;
	ret = lkmalloc(10, &buf, malloc_flags);
	if (assert_success) {
		return ret != -EINVAL ? EXIT_SUCCESS : EXIT_FAILURE;
	} else {
		return ret == -EINVAL ? EXIT_SUCCESS : EXIT_FAILURE;
	}
}
