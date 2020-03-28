#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#define CONVERT_NUM(s, ptr) if (sscanf((s), "%i", (ptr)) < 0) exit(EXIT_FAILURE)

#endif /* TEST_UTIL_H */
