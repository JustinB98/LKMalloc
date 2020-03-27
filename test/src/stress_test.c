#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lkmalloc.h"

int free_range(size_t **arr, int start, int end) {
	for (u_int i = start; i < end; ++i) {
		void *ptr = arr[i];
		int ret = lkfree(&ptr, LKF_REG);
		if (ret < 0) exit(EXIT_FAILURE);
	}
	return 0;
}

/* size */
int main(int argc, char *argv[]) {
	void *buf = NULL;
	if (argc != 2) return EXIT_FAILURE;
	u_int size;
	if (sscanf(argv[1], "%i", &size) < 0) return EXIT_FAILURE;
	int ret = lkmalloc(sizeof(size_t *) * size, &buf, LKM_REG);
	if (ret < 0) return EXIT_FAILURE;
	size_t **arr = buf;
	for (u_int i = 0; i < size; ++i) {
		void *s = NULL;
		ret = lkmalloc(sizeof(size_t *), &s, LKM_REG);
		if (ret < 0) return EXIT_FAILURE;
		arr[i] = s;
	}
	free_range(arr, size/4, size/2);
	free_range(arr, 3*size/4, size);
	free_range(arr, 0, size/4);
	free_range(arr, size/2, 3*size/4);
	ret = lkfree(&buf, LKF_REG);
	if (ret < 0) return EXIT_FAILURE;
	ret = lkreport(STDERR_FILENO, LKR_MATCH);
	return ret == (size * 2) + 2 ? EXIT_SUCCESS : EXIT_FAILURE;
}
