#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "lkmalloc.h"
#include "test_utils.h"

/* bin/shift_and_free size, malloc_flags, free_flags, offset */
int main(int argc, char *argv[]) {
	if (argc != 5) return EXIT_FAILURE;
	u_int size, malloc_flags, free_flags;
	int offset;
	CONVERT_NUM(argv[1], &size);
	CONVERT_NUM(argv[2], &malloc_flags);
	CONVERT_NUM(argv[3], &free_flags);
	CONVERT_NUM(argv[4], &offset);
	void *buf = NULL;
	int ret = lkmalloc(size, &buf, malloc_flags);
	if (ret < 0) return EXIT_FAILURE;
	buf += offset;
	ret = lkfree(&buf, free_flags);
	ret = lkreport(STDERR_FILENO, LKR_MATCH);
	return ret == 2 ? EXIT_SUCCESS : EXIT_FAILURE;
}
