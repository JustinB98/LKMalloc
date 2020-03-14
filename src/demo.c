#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "lkmalloc.h"

int main(int argc, char *argv[]) {
	char *s = "Hello World";
	void *new_s = NULL;
	printf("Requesting %lu bytes\n", strlen(s) + 1);
	int ret = lkmalloc(strlen(s) + 1, &new_s, LKM_INIT);
	printf("new_s right now: %p\n", new_s);
	printf("Result: %d\n", ret);
	char *n = new_s;
	strcpy(n, s);
	printf("String: %s\n", n);
	new_s += 4;
	ret = lkfree(&new_s, LKF_APPROX | LKF_WARN);
	printf("Free result: %d %d\n", ret, EINVAL);
}
