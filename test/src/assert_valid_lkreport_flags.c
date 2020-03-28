#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "lkmalloc.h"

/* assert_valid_lkreport_flags report_flags assert_success (0 will assert_fail) */
int main(int argc, char *argv[]) {
	if (argc != 3) return EXIT_FAILURE;
	u_int report_flags;
	int assert_success;
	int ret = sscanf(argv[1], "%i", &report_flags);
	assert_success = atoi(argv[2]);
	if (ret < 0) return EXIT_FAILURE;
	ret = lkreport(STDERR_FILENO, report_flags);
	if (assert_success) {
		return ret != -EINVAL ? EXIT_SUCCESS : EXIT_FAILURE;
	} else {
		return ret == -EINVAL ? EXIT_SUCCESS : EXIT_FAILURE;
	}
}
