#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/mman.h>

#include "lk_record.h"
#include "lkmalloc.h"
#include "lk_data.h"

static int initialized = 0;
static int page_size;

#define LK_REPORT_HEADER "record_type,filename,fxname,line_num,timestamp,ptr_passed,retval,size_or_flags,alloc_addr_returned\n"

static void lk_lib_fini() {
	lkreport(STDERR_FILENO, LKR_SERIOUS | LKR_BAD_FREE | LKR_ORPHAN_FREE | LKR_DOUBLE_FREE);
	lk_data_fini();
}

static int has_options_out_of_range(u_int largest_flag, u_int flags) {
	u_int largest_flag_mask = largest_flag | (largest_flag - 1);
	return (~largest_flag_mask) & flags;
}

static void insert_into_failed(LK_RECORD *record, int neg_code) {
	lk_record_set_retval(record, neg_code);
	lk_data_insert_failed_record(record);
}

static void init_if_needed() {
	if (!initialized) {
		page_size = getpagesize();
		if (page_size < 0) {
			perror("Could not get page size of system");
		}
		initialized = 1;
		lk_data_init();
		atexit(lk_lib_fini);
	}
}

static void fill_around_malloced_ptr(char *ptr, u_int actual_size, u_int flags) {
	if (flags & LKM_UNDER) memset(ptr, 0x6b, 8);
	if (flags & LKM_OVER) memset(ptr + actual_size - 8, 0x5a, 8);
}

static void *get_user_ptr(void *malloced_ptr, u_int flags) {
	if (malloced_ptr == NULL) return NULL;
	void *user_ptr = malloced_ptr;
	if (flags & LKM_UNDER) user_ptr = malloced_ptr + 8;
	return user_ptr;
}

static u_int get_actual_size(u_int size, u_int flags) {
	u_int new_size = size;
	if (flags & LKM_OVER) new_size += 8;
	if (flags & LKM_UNDER) new_size += 8;
	return new_size;
}

#ifdef EXTRA_CREDIT

static size_t get_mapped_size(u_int size) {
	if (size == 0) return 0;
	size_t mapped_size = 0;
	do {
		mapped_size += page_size;
	} while (mapped_size < size);
	return mapped_size;
}

static void *create_mapped_ptr(LK_RECORD *malloc_record, u_int size, u_int flags) {
	size_t mapped_size = get_mapped_size(size);
	if (mapped_size < 0) return NULL;
	size_t protected_block_size = 0;
	if (flags & LKM_PROT_BEFORE) protected_block_size += page_size;
	if (flags & LKM_PROT_AFTER) protected_block_size += page_size;
	void *ptr = mmap(NULL, mapped_size + protected_block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (flags & LKM_INIT) memset(ptr, 0, mapped_size + protected_block_size);
	lk_malloc_record_set_malloced_ptr(malloc_record, ptr);
	return ptr;
}

static int protect_blocks(void *ptr, size_t ptr_size, u_int flags) {
	int ret = 0;
	if (flags & LKM_PROT_BEFORE) {
		ret = mprotect(ptr, page_size, PROT_NONE);
		if (ret < 0) return ret;
		ptr += page_size;
	}
	if (flags & LKM_PROT_AFTER) {
		ret = mprotect(ptr + ptr_size, page_size, PROT_NONE);
		if (ret < 0) return ret;
	}
	return ret;
}

static int unmap_record(LK_RECORD *malloc_record) {
	size_t ptr_size = get_mapped_size(lk_malloc_record_get_size(malloc_record));
	void *ptr = lk_malloc_record_get_malloced_ptr(malloc_record);
	return munmap(ptr, ptr_size + page_size);
}

static void *get_mapped_ptr(LK_RECORD *malloc_record, u_int size, u_int flags) {
	void *ptr = create_mapped_ptr(malloc_record, size, flags);
	if (ptr == MAP_FAILED) return ptr;
	size_t ptr_size = get_mapped_size(size);
	int ret = protect_blocks(ptr, ptr_size, flags);
	if (ret < 0) {
		/* mprotect failed, try to clean up. If we can't unmap, then there's */
		/* nothing we can do */
		unmap_record(malloc_record);
		return NULL;
	}
	return ptr;
}

static void *get_mapped_user_ptr(void *mapped_ptr, size_t ptr_size, u_int size, u_int flags) {
	if (flags & LKM_PROT_BEFORE) return mapped_ptr + page_size;
	else if (flags & LKM_PROT_AFTER) return mapped_ptr + ptr_size - size;
	fprintf(stderr, "UNKNOWN FLAG MADE IT IN: %d\n", flags);
	return NULL;
}

static int insert_mapped_ptr(LK_RECORD *malloc_record, void **ptr, u_int size, u_int flags) {
	u_int actual_size = get_actual_size(size, flags);
	void *mapped_ptr;
	mapped_ptr = get_mapped_ptr(malloc_record, actual_size, flags);
	if (mapped_ptr == MAP_FAILED) {
		insert_into_failed(malloc_record, -errno);
		return -errno;
	} else {
		size_t ptr_size = get_mapped_size(actual_size);
		void *user_ptr = get_mapped_user_ptr(mapped_ptr, ptr_size, actual_size, flags);
		fill_around_malloced_ptr(user_ptr, actual_size, flags);
		user_ptr = get_user_ptr(user_ptr, flags);
		lk_malloc_record_set_addr_returned(malloc_record, user_ptr);
		*ptr = user_ptr;
		lk_data_insert_malloc_record(mapped_ptr, malloc_record);
		return 0;
	}
}

#endif

static void *get_ptr(u_int size, u_int flags) {
	u_int new_size = get_actual_size(size, flags);
	void *malloced_ptr = malloc(new_size);
	if (malloced_ptr == NULL) return NULL;
	if (flags & LKM_INIT) memset(malloced_ptr, 0, new_size);
	fill_around_malloced_ptr(malloced_ptr, new_size, flags);
	return malloced_ptr;
}

static void fill_metadata(LK_METADATA *metadata, int line_num, char *file_name, const char *function_name) {
	metadata->line_num = line_num;
	metadata->file_name = file_name;
	metadata->function_name = function_name;
	if (gettimeofday(&metadata->timestamp, NULL) < 0) {
		perror("gettimeofday");
	}
}

static void *malloc_space(LK_RECORD *malloc_record, u_int size, u_int flags) {
	void *malloced_ptr = get_ptr(size, flags);
	lk_malloc_record_set_malloced_ptr(malloc_record, malloced_ptr);
	void *user_ptr = get_user_ptr(malloced_ptr, flags);
	lk_malloc_record_set_addr_returned(malloc_record, user_ptr);
	return user_ptr;
}

static int insert_ptr(LK_RECORD *malloc_record, void **ptr, u_int size, u_int flags) {
	void *user_ptr = malloc_space(malloc_record, size, flags);
	if (user_ptr == NULL) {
		insert_into_failed(malloc_record, -errno);
		return -ENOMEM;
	}
	lk_data_insert_malloc_record(user_ptr, malloc_record);
	*ptr = user_ptr;
	return 0;
}

static int malloc_request_has_invalid_args(u_int flags, u_int size, void **ptr) {
	if (ptr == NULL) return 1;
	u_int largest_flag = LKM_UNDER;
#ifdef EXTRA_CREDIT
	largest_flag = LKM_PROT_BEFORE;
	size = get_actual_size(size, flags);
	int multiple_of_page_size = size & (page_size - 1);
	if ((flags & LKM_PROT_BEFORE) && (flags & LKM_PROT_AFTER) && multiple_of_page_size) return 1;
#endif
	if (has_options_out_of_range(largest_flag, flags)) return 1;
	return 0;
}

int __lkmalloc_internal(u_int size, void **ptr, u_int flags, char *file, const char *func, int line) {
	init_if_needed();
	LK_METADATA metadata;
	fill_metadata(&metadata, line, file, func);
	LK_RECORD *malloc_record = lk_create_malloc_record(flags, ptr, &metadata, size);
	if (malloc_record == NULL) return -ENOMEM;
	if (malloc_request_has_invalid_args(flags, size, ptr)) {
		insert_into_failed(malloc_record, -EINVAL);
		return -EINVAL;
	}
#ifdef EXTRA_CREDIT
	if (flags & (LKM_PROT_BEFORE | LKM_PROT_AFTER)) {
		return insert_mapped_ptr(malloc_record, ptr, size, flags);
	}
#endif
	return insert_ptr(malloc_record, ptr, size, flags);
}

static void terminate_program_if_error(u_int flags) {
	if (flags & LKF_ERROR) {
		fprintf(stderr, "LKF_ERROR - Terminating Program\n");
		exit(EXIT_FAILURE);
	}
}

static void data_not_found(void *ptr, u_int flags, char *file, const char *func, int line) {
	if (flags & LKF_UNKNOWN) {
		fprintf(stderr, "WARNING - Trying to free unknown %p in %s:%s:%d\n",
				ptr, file, func, line);
		terminate_program_if_error(flags);
	}
}

static int was_freed_in_redzone(LK_RECORD *malloc_record, void *ptr_requested, void *ptr_user_got) {
	u_int size = lk_malloc_record_get_size(malloc_record);
	if (ptr_requested < ptr_user_got) return 1;
	else if (ptr_user_got + size < ptr_requested) return 1;
	else return 0;
}

static int attempt_to_free(LK_RECORD *free_record, u_int flags, LK_RECORD *malloc_record, char *file, const char *func, int line) {
	/* If pointers are different, then LKF_APPROX was part of flags */
	void *ptr_requested = lk_free_record_get_ptr_requested(free_record);
	void *ptr_user_got = lk_free_record_get_user_ptr_returned(free_record);
	int was_freed_approx = ptr_requested != ptr_user_got;
	lk_malloc_record_set_freed_approx(malloc_record, was_freed_approx);
	if (was_freed_approx && (flags & LKF_WARN)) {
		fprintf(stderr, "WARNING - Freeing %p using LKF_APPROX. Pointer that was passed: %p %s:%s:%d\n",
				ptr_user_got, ptr_requested, file, func, line);
		terminate_program_if_error(flags);
	} else if ((flags & LKF_APPROX) &&
				was_freed_in_redzone(malloc_record, ptr_requested, ptr_user_got)) {
		fprintf(stderr, "WARNING - Freeing %p with LKF_APPROX but pointer passed (%p) was in the red zone."
						" %s:%s:%d\n", ptr_user_got, ptr_requested, file, func, line);
	}
#ifdef EXTRA_CREDIT
	u_int malloc_flags = lk_record_get_flags(malloc_record);
	if (malloc_flags & (LKM_PROT_BEFORE | LKM_PROT_AFTER)) {
		return unmap_record(malloc_record);
	}
#endif
	void *malloced_ptr = lk_malloc_record_get_malloced_ptr(malloc_record);
	free(malloced_ptr);
	return 0;
}

static int finder(void *fr, void *data) {
	LK_RECORD *free_record = fr;
	LK_RECORD *malloc_record = data;
	void *addr_returned = lk_malloc_record_get_addr_returned(malloc_record);
	void *ptr_requested = lk_free_record_get_ptr_requested(free_record);
	if (addr_returned == ptr_requested) {
		return 1;
	} else if ((lk_record_get_flags(free_record) & LKF_APPROX) == 0) {
		return 0;
	}
	u_int ptr_size = lk_malloc_record_get_size(malloc_record);
	u_int malloc_flags = lk_record_get_flags(malloc_record);
	void *start_ptr = addr_returned;
	if (malloc_flags & LKM_UNDER) start_ptr -= 8;
	void *end_ptr = start_ptr + ptr_size;
	if (malloc_flags & LKM_OVER) end_ptr += 8;
	void *ptr_to_find = ptr_requested;
	return start_ptr <= ptr_to_find && ptr_to_find < end_ptr;
}

static int free_request_has_invalid_args(void **ptr, u_int flags) {
	if (ptr == NULL) return 1;
	else if ((flags & LKF_WARN) && (flags && LKF_APPROX) == 0) return 1;
	/* If LKF_ERROR is present but not LKF_UNKNOWN or LKF_WARN, then it's invalid */
	else if ((flags & LKF_ERROR) && (flags && (LKF_UNKNOWN | LKF_WARN)) == 0) return 1;
	else if (has_options_out_of_range(LKF_ERROR, flags)) return 1;
	return 0;
}

int __lkfree_internal(void **ptr, u_int flags, char *file, const char *func, int line) {
	init_if_needed();
	LK_METADATA metadata;
	fill_metadata(&metadata, line, file, func);
	LK_RECORD *free_record = lk_create_record(1, flags, ptr, &metadata);
	if (free_record == NULL) return -ENOMEM;
	if (free_request_has_invalid_args(ptr, flags)) {
		insert_into_failed(free_record, -EINVAL);
		return -EINVAL;
	}
	lk_free_record_set_ptr_requested(free_record, *ptr);
	LK_RECORD *data_found = lk_data_insert_free_record(*ptr, free_record, finder);
	if (data_found == NULL) {
		data_not_found(*ptr, flags, file, func, line);
		/* Do not need to insert into failed list because lk_data_insert_free_record does this */
		lk_record_set_retval(free_record, -ENOENT);
		return -ENOENT;
	}
	int retval = attempt_to_free(free_record, flags, data_found, file, func, line);
	lk_record_set_retval(free_record, retval);
	return retval;
}

static int should_malloc_record_print(LK_RECORD *malloc_record, u_int flags) {
	int times_freed = lk_malloc_record_get_times_freed(malloc_record);
	int was_freed_approx = lk_malloc_record_was_freed_approx(malloc_record);
	if ((flags & LKR_SERIOUS) && times_freed == 0) return 1;
	else if ((flags & LKR_MATCH) && times_freed == 1) return 1;
	else if ((flags & LKR_DOUBLE_FREE) && times_freed > 1) return 1;
	else if ((flags & LKR_APPROX) && was_freed_approx) return 1;
	else return 0;
}

static int print_malloc_record(int fd, u_int flags, LK_RECORD *record) {
	if (!should_malloc_record_print(record, flags)) return 0;
	return lk_print_malloc_record(record, fd);
}

static int should_free_record_print(LK_RECORD *free_record, u_int flags) {
	void *ptr_user_got = lk_free_record_get_user_ptr_returned(free_record);
	void *ptr_requested = lk_free_record_get_ptr_requested(free_record);
	int was_freed_approx = ptr_requested != ptr_user_got;
	int time_freed = lk_free_record_get_times_freed(free_record);
	int was_orphaned = lk_free_record_is_orphaned_free(free_record);
	if ((flags & LKR_MATCH) && ptr_user_got != NULL) return 1;
	else if ((flags & (LKR_BAD_FREE | LKR_APPROX)) && was_freed_approx) return 1;
	else if ((flags & LKR_ORPHAN_FREE) && was_orphaned) return 1;
	else if ((flags & LKR_DOUBLE_FREE) && time_freed > 1) return 1;
	else return 0;
}

static int print_free_record(int fd, u_int flags, LK_RECORD *record) {
	if (!should_free_record_print(record, flags)) return 0;
	return lk_print_free_record(record, fd);
}

static int print_record(int fd, u_int flags, void *data) {
	LK_RECORD *record = data;
	int record_type = lk_record_get_record_type(record);
	if (record_type == 0) return print_malloc_record(fd, flags, record);
	else if (record_type == 1) return print_free_record(fd, flags, record);
	else fprintf(stderr, "ERROR UNKNOWN RECORD TYPE: %d\n", record_type);
	return -1;
}

static int print_header(int fd) {
	return dprintf(fd, LK_REPORT_HEADER);
}

static int report_request_has_invalid_args(u_int flags) {
	if ((flags & LKR_APPROX) && (flags & LKR_BAD_FREE)) return 1;
	else if (has_options_out_of_range(LKR_APPROX, flags)) return 1;
	return 0;
}

int __lkreport_internal(int fd, u_int flags) {
	init_if_needed();
	if (flags == LKR_NONE) return 0;
	else if (report_request_has_invalid_args(flags)) return -EINVAL;
	int result = print_header(fd);
	if (result < 0) goto __lk_report_internal_finish;
	result = lk_data_iterate_through_all_records(fd, flags, print_record);
__lk_report_internal_finish:
	return result < 0 ? -errno : result;
}
