#include <stdlib.h>
#include <stdio.h>

#include "lk_data.h"
#include "linked_list.h"
#include "lkmalloc.h"

static LINKED_LIST *list;

static void *current_ptr_to_find;
static u_int global_flags;
static int (*m_consumer)(LK_RECORD *);
static int (*f_consumer)(LK_RECORD *);

static int lk_finder(void *data) {
	LK_RECORD *record = data;
	if (record->record_type != 0) return 0;
	LK_MALLOC_RECORD *malloc_record = record->sub_record;
	if (malloc_record->addr_returned == current_ptr_to_find) return 1;
	else if ((global_flags & LKF_APPROX) == 0) return 0;
	u_int ptr_size = malloc_record->size;
	void *start_ptr = malloc_record->addr_returned;
	void *end_ptr = start_ptr + ptr_size;
	return start_ptr < current_ptr_to_find && current_ptr_to_find < end_ptr;
}

static void free_malloc_record(LK_MALLOC_RECORD *record) {
	if (record->times_freed == 0) {
		// free(record->malloced_ptr);
		fprintf(stderr, "WARNING - Memory leak! %p was never freed!\n", record->addr_returned);
	}
}

static void onRemoval(void *data) {
	LK_RECORD *record = data;
	if (record->record_type == 0) free_malloc_record(record->sub_record);
	free(record->sub_record);

	free(record);
}

void lk_data_init() {
	list = linked_list_init(onRemoval);	
}

static LK_RECORD *lk_data_insert(int record_type, int line_num, u_int flags, char *file, const char *function_name, void **ptr_passed, void *sub_record) {
	LK_RECORD *rec = malloc(sizeof(LK_RECORD));
	rec->record_type = record_type;
	rec->line_num = line_num;
	rec->flags = flags;
	rec->file_name = file;
	rec->function_name = function_name;
	rec->ptr_passed = ptr_passed;
	rec->sub_record = sub_record;
	struct timeval tv;
	int ret = gettimeofday(&tv, NULL);
	if (ret < 0) {
		perror("gettimeofday");
		return NULL;
	}
	rec->seconds = tv.tv_sec;
	rec->microseconds = tv.tv_usec;
	linked_list_insert(list, NULL, rec);
	return rec;
}

LK_RECORD *lk_data_insert_malloc_record(void **ptr_passed, u_int size, u_int flags, char *file, const char *function, int line) {
	LK_MALLOC_RECORD *malloc_record = malloc(sizeof(LK_MALLOC_RECORD));
	malloc_record->times_freed = 0;
	malloc_record->size = size;
	return lk_data_insert(0, line, flags, file, function, ptr_passed, malloc_record);
}

LK_RECORD *lk_data_insert_free_record(void **ptr_passed, void *ptr_requested, u_int flags, char *file, const char *function, int line) {
	LK_FREE_RECORD *free_record = malloc(sizeof(LK_FREE_RECORD));
	free_record->ptr_requested = ptr_requested;
	return lk_data_insert(1, line, flags, file, function, ptr_passed, free_record);
}

LK_RECORD *lk_data_find_malloc_record(LK_FREE_RECORD *free_record, void *ptr, u_int flags) {
	current_ptr_to_find = ptr;
	global_flags = flags;
	LK_RECORD *result = linked_list_find(list, lk_finder);
	if (result != NULL) {
		LK_MALLOC_RECORD *malloc_record = result->sub_record;
		free_record->ptr_freed = malloc_record->addr_returned;
	}
	current_ptr_to_find = NULL;
	global_flags = 0;
	return result;
}

void lk_data_remove(void *ptr) {
	linked_list_remove(list, ptr);
}

static int lk_consumer(void *data) {
	LK_RECORD *rec = data;
	int ret = 0;
	if (rec->record_type == 0) {
		ret = m_consumer(rec);
	} else {
		ret = f_consumer(rec);
	}
	return ret < 0 ? 1 : 0;
}

int lk_data_for_each(int (*mal_consumer)(LK_RECORD *), int (*free_consumer)(LK_RECORD *)) {
	m_consumer = mal_consumer;
	f_consumer = free_consumer;
	/* Taking advantage that we can pass a function into the finder */
	void *returned = linked_list_find(list, lk_consumer);
	// NULL means entire process completed
	// Non null means the process was interuptted by a failed read
	return returned == NULL ? 0 : -1;
}

void lk_data_fini() {
	linked_list_fini(list);
}
