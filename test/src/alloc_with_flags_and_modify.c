#include <stdio.h>
#include <unistd.h>

#include "lkmalloc.h"

/* size, malloc flags, free flags, buffer offset */
int main(int argc, char *argv[]) {
	if (argc != 5) return EXIT_FAILURE;
	u_int size = atoi(argv[1]);
	u_int malloc_flags = atoi(argv[2]);
	u_int free_flags = atoi(argv[3]);
	int buffer_offset = atoi(argv[4]);
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
