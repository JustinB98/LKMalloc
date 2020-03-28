#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "lkmalloc.h"
#include "test_utils.h"

/* size, malloc flags, free flags, buffer offset */
int main(int argc, char *argv[]) {
	if (argc != 5) return EXIT_FAILURE;
	u_int size, malloc_flags, free_flags;
	int buffer_offset;
	CONVERT_NUM(argv[1], &size);
	CONVERT_NUM(argv[2], &malloc_flags);
	CONVERT_NUM(argv[3], &free_flags);
	CONVERT_NUM(argv[4], &buffer_offset);
	printf("buffer offset %d\n", buffer_offset);
	void *buf = NULL;
	int ret = lkmalloc(size, &buf, malloc_flags);
	if (ret < 0) return EXIT_FAILURE;
	char *a = buf + buffer_offset;
	*a = 0;
	ret = lkfree(&buf, free_flags);
	if (ret < 0) return EXIT_FAILURE;
	ret = lkreport(STDERR_FILENO, LKR_MATCH);
	return ret == 2 ? EXIT_SUCCESS : EXIT_FAILURE;
}
