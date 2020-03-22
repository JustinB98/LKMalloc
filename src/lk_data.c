#include <stdlib.h>
#include <stdio.h>

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
	void *malloced_ptr = mal_record->sub_record.malloc_record.malloced_ptr;
	free_record->sub_record.free_record.ptr_freed = malloced_ptr;
	++(mal_record->sub_record.malloc_record.times_freed);
	free_record->sub_record.free_record.n_time_free = mal_record->sub_record.malloc_record.times_freed;
	binary_tree_remove(active_records, mal_record);
	binary_tree_replace(completed_records, malloced_ptr, mal_record);
	return malloced_ptr;
}

static void mark_as_double_free(LK_RECORD *free_record, LK_RECORD *malloc_record) {
	int times_freed = malloc_record->sub_record.malloc_record.times_freed + 1;
	malloc_record->sub_record.malloc_record.times_freed = times_freed;
	free_record->sub_record.free_record.n_time_free = times_freed;
	free_record->sub_record.free_record.orphaned_free = 0;
}

static void mark_as_orphaned_free(LK_RECORD *free_record) {
	free_record->sub_record.free_record.n_time_free = 0;
	free_record->sub_record.free_record.orphaned_free = 1;
}

void *lk_data_insert_free_record(void *ptr, LK_RECORD *free_record, int (*finder)(void *, void *)) {
	linked_list_insert(all_records_list, free_record);
	LK_RECORD *mal_record = binary_tree_find_with_function(active_records, ptr, free_record, finder);
	if (mal_record != NULL) {
		free_record->sub_record.free_record.n_time_free = 1;
		free_record->sub_record.free_record.orphaned_free = 0;
		return mark_as_complete(free_record, mal_record);
	}
	mal_record = binary_tree_find_with_function(completed_records, ptr, free_record, finder);
	if (mal_record != NULL) {
		mark_as_double_free(free_record, mal_record);
		return NULL;
	}
	mark_as_orphaned_free(free_record);
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
	if (record->record_type == 0) {
		free_malloc_record(record);
	} else if (record->record_type == 1) {
		free_free_record(record);
	} else {
		fprintf(stderr, "WARNING - Unknown record trying to be freed");
	}
}

void lk_data_fini() {
	binary_tree_fini(active_records, NULL);
	binary_tree_fini(completed_records, NULL);
	linked_list_fini(all_records_list, onRemoval);
}
