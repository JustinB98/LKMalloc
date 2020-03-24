#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "lkmalloc.h"
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

static void fill_metadata(LK_METADATA *metadata, int line_num, char *file_name, const char *function_name) {
	metadata->line_num = line_num;
	metadata->file_name = file_name;
	metadata->function_name = function_name;
	if (gettimeofday(&metadata->timestamp, NULL) < 0) {
		perror("gettimeofday");
	}
}

static LK_RECORD *create_record(LK_METADATA *metadata) {
	LK_RECORD *record = malloc(sizeof(LK_RECORD));
	if (record != NULL) {
		record->metadata = *metadata;
	}
	return record;
}

static LK_RECORD *create_malloc_record(LK_METADATA *metadata, void *ptr, u_int flags, u_int size) {
	LK_RECORD *record = create_record(metadata);
	if (record != NULL) {
		record->record_type = 0;
		record->ptr_passed = ptr;
		record->flags = flags;
		record->sub_record.malloc_record.size = size;
		record->sub_record.malloc_record.times_freed = 0;
#if EXTRA_CREDIT
		record->sub_record.malloc_record.was_mmapped = 0;
#endif
	}
	return record;
}

static LK_RECORD *create_free_record(LK_METADATA *metadata) {
	LK_RECORD *record = create_record(metadata);
	if (record != NULL) {
		record->record_type = 1;
	}
	return record;
}

static void *malloc_space(LK_RECORD *malloc_record, u_int size, u_int flags) {
	void *malloced_ptr = get_ptr(size, flags);
	malloc_record->sub_record.malloc_record.malloced_ptr = malloced_ptr;
	void *user_ptr = get_user_ptr(malloced_ptr, flags);
	malloc_record->sub_record.malloc_record.addr_returned = user_ptr;
	return user_ptr;
}

int __lkmalloc_internal(u_int size, void **ptr, u_int flags, char *file, const char *func, int line) {
	init_if_needed();
	LK_METADATA metadata;
	fill_metadata(&metadata, line, file, func);
	LK_RECORD *malloc_record = create_malloc_record(&metadata, *ptr, flags, size);
	void *user_ptr = malloc_space(malloc_record, size, flags);
	if (user_ptr == NULL) {
		malloc_record->retval = -errno;
		lk_data_insert_failed_record(malloc_record);
		return -errno;
	}
	lk_data_insert_malloc_record(user_ptr, malloc_record);
	*ptr = user_ptr;
	malloc_record->retval = 0;
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

static void attempt_to_free(LK_RECORD *free_record, u_int flags) {
	/* If pointers are different, then LKF_APPROX was part of flags */
	if (free_record->sub_record.free_record.ptr_requested != free_record->sub_record.free_record.ptr_freed && (flags & LKF_WARN)) {
		fprintf(stderr, "WARNING - Freeing a pointer using LKF_APPROX\n");
		terminate_program_if_error(flags);
	}
	free(free_record->sub_record.free_record.ptr_freed);
}

static int finder(void *fr, void *data) {
	LK_RECORD *free_record = fr;
	LK_RECORD *malloc_record = data;
	if (malloc_record->sub_record.malloc_record.addr_returned == free_record->sub_record.free_record.ptr_requested) {
		return 1;
	} else if ((free_record->flags & LKF_APPROX) == 0) {
		return 0;
	}
	u_int ptr_size = malloc_record->sub_record.malloc_record.size;
	void *start_ptr = malloc_record->sub_record.malloc_record.addr_returned;
	void *end_ptr = start_ptr + ptr_size;
	void *ptr_to_find = free_record->sub_record.free_record.ptr_requested;
	return start_ptr < ptr_to_find && ptr_to_find < end_ptr;
}

int __lkfree_internal(void **ptr, u_int flags, char *file, const char *func, int line) {
	init_if_needed();
	LK_METADATA metadata;
	fill_metadata(&metadata, line, file, func);
	LK_RECORD *free_record = create_free_record(&metadata);
	free_record->ptr_passed = *ptr;
	free_record->flags = flags;
	free_record->sub_record.free_record.ptr_requested = *ptr;
	free_record->sub_record.free_record.ptr_freed = NULL;
#ifdef EXTRA_CREDIT
	free_record->sub_record.free_record.was_mmapped = 0;
#endif
	if (ptr == NULL || *ptr == NULL) {
		free_record->retval = -EINVAL;
		lk_data_insert_failed_record(free_record);
		return -EINVAL;
	}
	void *data_found = lk_data_insert_free_record(*ptr, free_record, finder);
	if (data_found == NULL) {
		data_not_found(flags, file, func, line);
		free_record->retval = -EINVAL;
		return -EINVAL;
	}
	attempt_to_free(free_record, flags);
	free_record->retval = 0;
	return 0;
}

static int write_buf(int fd, void *buf, size_t size) {
	ssize_t result = 0;
	ssize_t total_written = 0;
	while (total_written < size) {
		result = write(fd, buf, size);
		if (result < 0) break;
		total_written += result;
		buf += result;
	}
	return result < 0 ? -1 : 0;
}

static int should_malloc_record_print(LK_MALLOC_RECORD *malloc_record, u_int flags) {
	if ((flags & LKR_SERIOUS) && malloc_record->times_freed == 0) return 1;
	else if ((flags & LKR_MATCH) && malloc_record->times_freed == 1) return 1;
	else if ((flags & LKR_DOUBLE_FREE) && malloc_record->times_freed > 1) return 1;
	else return 0;
}

static int lk_print_malloc_record(int fd, u_int flags, LK_RECORD *record) {
	if (!should_malloc_record_print(&record->sub_record.malloc_record, flags)) return 0;
	LK_MALLOC_RECORD *malloc_record = &record->sub_record.malloc_record;
	return dprintf(fd, "%d,%s,%s,%d,%lu,%p,%d,%u,%p\n",
				   record->record_type, record->metadata.file_name, record->metadata.function_name,
				   record->metadata.line_num, record->metadata.timestamp.tv_sec + record->metadata.timestamp.tv_usec,
				   record->ptr_passed, record->retval, malloc_record->size,
				   malloc_record->addr_returned);
}

static int should_free_record_print(LK_FREE_RECORD *free_record, u_int flags) {
	if ((flags & LKR_MATCH) && free_record->ptr_freed != NULL) return 1;
	else if ((flags & LKR_BAD_FREE) &&
			  free_record->ptr_requested != free_record->ptr_freed) return 1;
	else if ((flags & LKR_ORPHAN_FREE) && free_record->ptr_freed == NULL) return 1;
	else return 0;
}

static int lk_print_free_record(int fd, u_int flags, LK_RECORD *record) {
	if (!should_free_record_print(&record->sub_record.free_record, flags)) return 0;
	return dprintf(fd, "%d,%s,%s,%d,%lu,%p,%d,%u\n",
				   record->record_type, record->metadata.file_name, record->metadata.function_name,
				   record->metadata.line_num, record->metadata.timestamp.tv_sec + record->metadata.timestamp.tv_usec,
				   record->ptr_passed, record->retval, record->flags);
}

static int print_record(int fd, u_int flags, void *data) {
	LK_RECORD *record = data;
	if (record->record_type == 0) return lk_print_malloc_record(fd, flags, record);
	else if (record->record_type == 1) return lk_print_free_record(fd, flags, record);
	else fprintf(stderr, "ERROR UNKNOWN RECORD TYPE\n");
	return -1;
}

static int print_header(int fd) {
	char *header = "record_type,filename,fxname,line_num,timestamp,ptr_passed,retval,size_or_flags,alloc_addr_returned\n";
	return write_buf(fd, header, strlen(header));
}

int __lkreport_internal(int fd, u_int flags) {
	init_if_needed();
	if (flags == LKR_NONE) return 0;
	int result = print_header(fd);
	if (result < 0) goto __lk_report_internal_finish;
	result = lk_data_iterate_through_all_records(fd, flags, print_record);
__lk_report_internal_finish:
	return result < 0 ? -errno : result;
}
