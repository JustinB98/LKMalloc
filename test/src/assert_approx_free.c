#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "lkmalloc.h"
#include "test_utils.h"

/* assert_approx_frees frees */
int main(int argc, char *argv[]) {
	if (argc != 2) return EXIT_FAILURE;
	u_int records;
	CONVERT_NUM(argv[1], &records);
	for (u_int i = 0; i < records; ++i) {
		void *buf = NULL;
		int ret = lkmalloc(10, &buf, LKM_REG);
		if (ret < 0) return EXIT_FAILURE;
		buf += 5;
		ret = lkfree(&buf, LKF_APPROX);
		if (ret < 0) return EXIT_FAILURE;
	}
	int num_of_records = lkreport(STDERR_FILENO, LKR_APPROX);
	return (num_of_records == (records << 1)) ? EXIT_SUCCESS : EXIT_FAILURE;
}
