#ifndef LK_RECORD_H
#define LK_RECORD_H

#include <sys/time.h>

/* Created as a separate struct to be able to create it separately */
typedef struct lk_metadata {
	int line_num; /* Line number where the lk* function was called */
	char *file_name; /* File name where lk* function was called */
	const char *function_name; /* Function where lk* function was called */
	struct timeval timestamp; /* Time struct that will be fill with data from gettimeofday(2) */
} LK_METADATA;

/* Created as a separate record to save space */
typedef struct lk_malloc_record {
	int times_freed; /* Times this pointer was freed */
	int freed_approx; /* Boolean value if this was freed with LKR_APPROX */
	u_int size; /* Size of the pointer requested */
	void *addr_returned; /* Ptr that gets returned to the user; used as key to the data structures */
	void *malloced_ptr; /* Ptr that malloc/mmap returned */
} LK_MALLOC_RECORD;

/* Created as a separate record to save space */
typedef struct lk_free_record {
	void *ptr_requested; /* Pointer requested to be freed (Pointer that lkfree parameter is pointing to) */
	void *user_ptr_returned; /* Ptr that lkmalloc returned */
	int times_freed; /* The nth time this pointer was freed. If 1, then it's a perfect match. If 2, it was a double free */
	int orphaned_free; /* Boolean value if this record was a result of an orphaned free */
} LK_FREE_RECORD;

/* Record structure to be put into the data structures */
typedef struct lk_record {
	LK_METADATA metadata; /* See lk_metadata above */
	int record_type; /* Record type 0 for malloc record, 1 for free */
	int retval; /* Return value of lkmalloc or lkfree */
	void *ptr_passed; /* Pointer passed to lkmalloc and lkfree */
	u_int flags; /* Flags passed into lkmalloc or lkfree */
	/* Either holds a malloc record or free record */
	/* To find out which, check the record_type */
	union {
		LK_MALLOC_RECORD malloc_record;
		LK_FREE_RECORD free_record;
	} sub_record;
} LK_RECORD;

/* Creates a record and memsets the sub_record field */
LK_RECORD *lk_create_record(int record_type, u_int flags, void **ptr_passed, LK_METADATA *metadata);

/* Creates a malloc record */
LK_RECORD *lk_create_malloc_record(u_int flags, void **ptr_passed, LK_METADATA *metadata, u_int size);

/**
 * Naming convention is as following:
 * lk_record for lk_record fields, lk_malloc_record for lk_malloc record fields, lk_free_record for lk_free record fields
 * get for getting a value, set for setting a value (requires an extra parameter to set it)
 * field name, which is the field the caller wants to get or set
 * All these functions need the record to know which record to modify
 */

u_int lk_record_get_flags(LK_RECORD *record);

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

int lk_malloc_record_was_freed_approx(LK_RECORD *record);
void lk_malloc_record_set_freed_approx(LK_RECORD *record, int was_free_approx);

void *lk_free_record_get_ptr_requested(LK_RECORD *record);
void lk_free_record_set_ptr_requested(LK_RECORD *record, void *ptr_requested);

void *lk_free_record_get_user_ptr_returned(LK_RECORD *record);
void lk_free_record_set_user_ptr_returned(LK_RECORD *record, void *ptr_returned);

int lk_free_record_get_times_freed(LK_RECORD *record);
void lk_free_record_set_times_freed(LK_RECORD *record, int times_freed);

int lk_free_record_is_orphaned_free(LK_RECORD *record);
void lk_free_record_set_orphaned_free(LK_RECORD *record, int orphaned_free);

int lk_record_get_record_type(LK_RECORD *record);

void *lk_record_get_ptr_passed(LK_RECORD *record);

int lk_record_get_retval(LK_RECORD *record);
void lk_record_set_retval(LK_RECORD *record, int retval);

/**
 * Prints malloc record in specified format according to hw2.pdf
 * @param record	record to print
 * @param fd		file descriptor to print to
 * @return			dprintf result
 */
int lk_print_malloc_record(LK_RECORD *record, int fd);

/**
 * Prints free record in specified format according to hw2.pdf
 * @param record	record to print
 * @param fd		file descriptor to print to
 * @return			dprintf result
 */
int lk_print_free_record(LK_RECORD *record, int fd);

#endif /* LK_RECORD_H */
