#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "lkmalloc.h"

/* bin/assert_orphan_free frees */
int main(int argc, char *argv[]) {
	if (argc != 2) return EXIT_FAILURE;
	u_int frees;
	int ret = sscanf(argv[1], "%i", &frees);
	if (ret < 0) return EXIT_FAILURE;
	void *buf = NULL;
	for (u_int i = 0; i < frees; ++i) {
		lkfree(&buf, LKF_REG);
	}
	ret = lkreport(STDERR_FILENO, LKR_ORPHAN_FREE);
	return ret == frees ? EXIT_SUCCESS : EXIT_FAILURE;
}
