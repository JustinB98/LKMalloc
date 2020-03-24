#ifndef LK_RECORD_H
#define LK_RECORD_H

#include <sys/time.h>

typedef struct lk_metadata {
	int line_num;
	char *file_name;
	const char *function_name;
	struct timeval timestamp;
} LK_METADATA;

typedef struct lk_malloc_record {
	int times_freed;
	u_int size;
	void *addr_returned;
	void *malloced_ptr;
} LK_MALLOC_RECORD;

typedef struct lk_free_record {
	void *ptr_requested;
	void *ptr_freed;
	int times_freed, orphaned_free;
} LK_FREE_RECORD;

typedef struct lk_record {
	LK_METADATA metadata;
	int record_type, retval;
	void *ptr_passed;
	u_int flags;
	union {
		LK_MALLOC_RECORD malloc_record;
		LK_FREE_RECORD free_record;
	} sub_record;
} LK_RECORD;

LK_RECORD *lk_record_create(int record_type, u_int flags, void **ptr_passed, LK_METADATA *metadata);

int lk_record_get_line_num(LK_RECORD *record);

char *lk_record_get_file_name(LK_RECORD *record);

const char *lk_record_get_function_name(LK_RECORD *record);

size_t lk_record_get_timestamp(LK_RECORD *record);

int lk_malloc_record_get_times_freed(LK_RECORD *record);
void lk_malloc_record_increment_times_freed(LK_RECORD *record);

u_int lk_malloc_record_get_size(LK_RECORD *record);
void lk_malloc_record_set_size(LK_RECORD *record, u_int size);

void *lk_malloc_record_get_addr_returned(LK_RECORD *record);
void lk_malloc_record_set_addr_returned(LK_RECORD *record, void *addr);

void *lk_malloc_record_get_malloced_ptr(LK_RECORD *record);
void lk_malloc_record_set_malloced_ptr(LK_RECORD *record, void *malloced_ptr);

void *lk_free_record_get_ptr_requested(LK_RECORD *record);
void lk_free_record_set_ptr_requested(LK_RECORD *record, void *ptr_requested);

void *lk_free_record_get_ptr_freed(LK_RECORD *record);
void lk_free_record_set_ptr_freed(LK_RECORD *record, void *ptr_freed);

int lk_free_record_get_times_freed(LK_RECORD *record);
void lk_free_record_increment_times_freed(LK_RECORD *record);

int lk_free_record_is_orphaned_free(LK_RECORD *record);
void lk_free_record_set_orphaned_free(LK_RECORD *record, int orphaned_free);

int lk_record_get_record_type(LK_RECORD *record);

void *lk_record_get_ptr_passed(LK_RECORD *record);

int lk_record_get_retval(LK_RECORD *record);
void lk_record_set_retval(LK_RECORD *record, int retval);

#endif /* LK_RECORD_H */
