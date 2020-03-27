#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "lkmalloc.h"

void convert_to_num(char *s, u_int *p) {
	if (sscanf(s, "%i", p) < 0) exit(EXIT_FAILURE);
}

/* size, malloc_flags, free_flags, offset */
int main(int argc, char *argv[]) {
	if (argc != 5) return EXIT_FAILURE;
	u_int size, malloc_flags, free_flags;
	int offset;
	convert_to_num(argv[1], &size);
	convert_to_num(argv[2], &malloc_flags);
	convert_to_num(argv[3], &free_flags);
	if (sscanf(argv[4], "%i", &offset) < 0) return EXIT_FAILURE;
	void *buf = NULL;
	int ret = lkmalloc(size, &buf, malloc_flags);
	if (ret < 0) return EXIT_FAILURE;
	buf += offset;
	ret = lkfree(&buf, free_flags);
	ret = lkreport(STDERR_FILENO, LKR_MATCH);
	return ret == 2 ? EXIT_SUCCESS : EXIT_FAILURE;
}
