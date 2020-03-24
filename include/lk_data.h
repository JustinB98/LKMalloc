#ifndef LK_DATA_H
#define LK_DATA_H

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
#ifdef EXTRA_CREDIT
	int was_mmapped;
#endif
} LK_MALLOC_RECORD;

typedef struct lk_free_record {
	void *ptr_requested;
	void *ptr_freed;
	int n_time_free, orphaned_free;
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

void lk_data_init();

void lk_data_insert_failed_record(LK_RECORD *record);

/**
 * Will insert malloc record into pending list
 */
void lk_data_insert_malloc_record(void *ptr, LK_RECORD *malloc_record);

/**
 * Will attempt to look for a malloc record in the active heap
 * If it can't, then the completed records need to be searched
 */
void *lk_data_insert_free_record(void *ptr, LK_RECORD *free_record, int (*finder)(void *, void *));

int lk_data_iterate_through_all_records(int fd, u_int flags, int (*consumer)(int, u_int, void *));

void lk_data_fini();

#endif /* LK_DATA_H */
