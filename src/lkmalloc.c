#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "lk_lib.h"
#include "lk_data.h"

static int initialized = 0;

static void lk_lib_fini() {
	printf("Finishing\n");
	lk_data_fini();
}

static void init_if_needed() {
	if (!initialized) {
		initialized = 1;
		lk_data_init();
		atexit(lk_lib_fini);
	}
}

static void fill_around_malloced_ptr(char *ptr, u_int size, u_int flags) {
	if (flags & LKM_UNDER) *ptr = 0x6b;
	if (flags & LKM_OVER) ptr[size] = 0x5a;
}

static u_int get_actual_size(u_int size, u_int flags) {
	u_int new_size = size;
	if (flags & LKM_OVER) new_size += 8;
	if (flags & LKM_UNDER) new_size += 8;
	return new_size;
}

static void *get_ptr(u_int size, u_int flags) {
	u_int new_size = get_actual_size(size, flags);
	void *malloced_ptr = malloc(new_size);
	if (malloced_ptr == NULL) return NULL;
	if (flags & LKM_INIT) memset(malloced_ptr, 0, size);
	fill_around_malloced_ptr(malloced_ptr, size, flags);
	return malloced_ptr;
}

static void *get_user_ptr(void *malloced_ptr, u_int flags) {
	if (malloced_ptr == NULL) return NULL;
	void *user_ptr = malloced_ptr;
	if (flags & LKM_UNDER) user_ptr = malloced_ptr + 8;
	return user_ptr;
}

int __lkmalloc_internal(u_int size, void **ptr, u_int flags, char *file, const char *func, int line) {
	init_if_needed();
	void *malloced_ptr = get_ptr(size, flags);
	if (malloced_ptr == NULL) return -errno;
	lk_data_insert(malloced_ptr, size, flags, file, func, line);
	*ptr = get_user_ptr(malloced_ptr, flags);
	return 0;
}

static void terminate_program_if_error(u_int flags) {
	if (flags & LKF_ERROR) {
		exit(EXIT_FAILURE);
	}
}

static void data_not_found(u_int flags, char *file, const char *func, int line) {
	if (flags & LKF_UNKNOWN) {
		fprintf(stderr, "Trying to allocate a pointer that was never lkmalloced\n");
	}
	terminate_program_if_error(flags);
}

static int already_free(LK_DATA *data_found, u_int flags) {
	fprintf(stderr, "%p was already freed\n", data_found->ptr);
	return 0;
}

static int attempt_to_free(LK_DATA *data_found, u_int flags, void *original_ptr) {
	int was_freed = data_found->freed != 0;
	++(data_found->freed);
	if (was_freed) {
		already_free(data_found, flags);
		return -EINVAL;
	}
	/* If pointers are different, then LKF_APPROX was part of flags */
	if (data_found->ptr != original_ptr && (flags & LKF_WARN)) {
		fprintf(stderr, "WARNING - Freeing a pointer using LKF_APPROX\n");
		terminate_program_if_error(flags);
	}
	void *ptr_to_free = data_found->ptr;
	if (data_found->flags & LKM_OVER) ptr_to_free -= 8;
	free(ptr_to_free);
	return 0;
}

int __lkfree_internal(void **ptr, u_int flags, char *file, const char *func, int line) {
	init_if_needed();
	LK_DATA *data_found = lk_data_find(*ptr, flags);
	if (data_found == NULL) {
		data_not_found(flags, file, func, line);
		return -EINVAL;
	}
	return attempt_to_free(data_found, flags, *ptr);
}
