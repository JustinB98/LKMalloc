#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "lkmalloc.h"

/* times_to_free */
int main(int argc, char *argv[]) {
	if (argc != 2) return EXIT_FAILURE;
	u_int times_to_free;
	if (sscanf(argv[1], "%i", &times_to_free) < 0) return EXIT_FAILURE;
	void *buf = NULL;
	int ret = lkmalloc(10, &buf, LKM_REG);
	if (ret < 0) return EXIT_FAILURE;
	for (u_int i = 0; i < times_to_free; ++i) {
		/* do not check retval */
		/* don't want to stop the program in case of lkfree internal error */
		/* Just checking for double frees */
		lkfree(&buf, LKF_REG);
	}
	ret = lkreport(STDERR_FILENO, LKR_DOUBLE_FREE);
	/* Except only first time to be valid. Malloc record gets included */
	/* This makes times_to_free - 1 double frees and +1 for the malloc record */
	int assertion;
	if (times_to_free == 1) assertion = ret == 0;
	else assertion = ret == times_to_free;
	return assertion ? EXIT_SUCCESS : EXIT_FAILURE;
}
