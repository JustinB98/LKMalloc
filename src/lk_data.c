#include <stdlib.h>
#include <stdio.h>

#include "lk_record.h"
#include "lk_data.h"
#include "linked_list.h"
#include "binary_tree.h"

static LINKED_LIST *all_records_list;
static BINARY_TREE *completed_records;
static BINARY_TREE *active_records;

void lk_data_init() {
	all_records_list = linked_list_init();
	completed_records = binary_tree_init();
	active_records = binary_tree_init();
}

void lk_data_insert_malloc_record(void *ptr, LK_RECORD *malloc_record) {
	linked_list_insert(all_records_list, malloc_record);
	binary_tree_insert(active_records, ptr, malloc_record);
}

void lk_data_insert_failed_record(LK_RECORD *record) {
	linked_list_insert(all_records_list, record);
}

static void *mark_as_complete(LK_RECORD *free_record, LK_RECORD *mal_record) {
	void *addr_returned = lk_malloc_record_get_addr_returned(mal_record);
	lk_free_record_set_user_ptr_returned(free_record, addr_returned);
	lk_malloc_record_increment_times_freed(mal_record);
	lk_free_record_set_times_freed(free_record, 1);
	binary_tree_remove(active_records, lk_malloc_record_get_addr_returned(mal_record));
	binary_tree_replace(completed_records, addr_returned, mal_record);
	return mal_record;
}

static void mark_as_double_free(LK_RECORD *free_record, LK_RECORD *malloc_record) {
	lk_malloc_record_increment_times_freed(malloc_record);
	int times_freed = lk_malloc_record_get_times_freed(malloc_record);
	lk_free_record_set_times_freed(free_record, times_freed);
}

LK_RECORD *lk_data_insert_free_record(void *ptr, LK_RECORD *free_record, int (*finder)(void *, void *)) {
	linked_list_insert(all_records_list, free_record);
	LK_RECORD *mal_record = binary_tree_find_with_function(active_records, ptr, free_record, finder);
	if (mal_record != NULL) {
		return mark_as_complete(free_record, mal_record);
	}
	mal_record = binary_tree_find_with_function(completed_records, ptr, free_record, finder);
	if (mal_record != NULL) {
		mark_as_double_free(free_record, mal_record);
		return NULL;
	}
	lk_free_record_set_orphaned_free(free_record, 1);
	return NULL;
}

static void free_malloc_record(LK_RECORD *malloc_record) {
	free(malloc_record);
}

static void free_free_record(LK_RECORD *free_record) {
	free(free_record);
}

static void onRemoval(void *data) {
	LK_RECORD *record = data;
	int record_type = lk_record_get_record_type(record);
	if (record_type == 0) {
		free_malloc_record(record);
	} else if (record_type == 1) {
		free_free_record(record);
	} else {
		fprintf(stderr, "WARNING - Unknown record trying to be freed: %d\n", record_type);
	}
}

int lk_data_iterate_through_all_records(int fd, u_int flags, int (*consumer)(int, u_int, void *)) {
	return linked_list_iterate_with_count(all_records_list, fd, flags, consumer);
}

void lk_data_fini() {
	binary_tree_fini(active_records, NULL);
	binary_tree_fini(completed_records, NULL);
	linked_list_fini(all_records_list, onRemoval);
}
