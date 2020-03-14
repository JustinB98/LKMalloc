#ifndef LK_DATA_H
#define LK_DATA_H

#include <sys/time.h>

typedef struct lk_record {
	int record_type, line_num, retval;
	u_int flags;
	char *file_name;
	const char *function_name;
	void **ptr_passed;
	time_t seconds;
	suseconds_t microseconds;
	void *sub_record; /* Used for either lk_malloc_record or lk_free_record */
} LK_RECORD;

typedef struct lk_malloc_record {
	int times_freed;
	u_int size;
	void *addr_returned;
	void *malloced_ptr;
} LK_MALLOC_RECORD;

typedef struct lk_free_record {
	void *ptr_requested;
	void *ptr_freed; /* NULL if no ptr was freed */
} LK_FREE_RECORD;

void lk_data_init();

LK_RECORD *lk_data_insert_malloc_record(void **ptr_passed, u_int size, u_int flags, char *file, const char *function, int line);
LK_RECORD *lk_data_insert_free_record(void **ptr_passed, void *ptr_freed, u_int flags, char *file, const char *func, int line);

LK_RECORD *lk_data_find_malloc_record(LK_FREE_RECORD *free_record, void *ptr, u_int flags);

void lk_data_remove(void *ptr);

void lk_data_fini();

#endif /* LK_DATA_H */
