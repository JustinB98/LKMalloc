#include <stdlib.h>
#include <stdio.h>

#include "lk_data.h"
#include "linked_list.h"
#include "lkmalloc.h"

static LINKED_LIST *list;

static void *current_ptr_to_find;
static u_int global_flags;

static int lk_finder(void *data) {
	LK_DATA *lk_data = data;
	if (lk_data->ptr == current_ptr_to_find) return 1;
	else if ((global_flags & LKF_APPROX) == 0) return 0;
	u_int ptr_size = lk_data->size;
	void *start_ptr = lk_data->ptr;
	void *end_ptr = start_ptr + ptr_size;
	return start_ptr < current_ptr_to_find && current_ptr_to_find < end_ptr;
}

static void onRemoval(void *data) {
	LK_DATA *lk_data = data;
	if (lk_data->freed == 0) {
		free(lk_data->ptr);
	}
	free(lk_data);
}

void lk_data_init() {
	list = linked_list_init(onRemoval);	
}

void lk_data_insert(void *ptr, u_int size, u_int flags, char *file, const char *func, int line) {
	LK_DATA *lk_data = malloc(sizeof(LK_DATA));
	lk_data->ptr = ptr;
	lk_data->size = size;
	lk_data->flags = flags;
	lk_data->freed = 0;
	lk_data->file = file;
	lk_data->func = func;
	lk_data->line = line;
	linked_list_insert(list, ptr, lk_data);
}

LK_DATA *lk_data_find(void *ptr, u_int flags) {
	current_ptr_to_find = ptr;
	global_flags = flags;
	LK_DATA *result = linked_list_find(list, lk_finder);
	current_ptr_to_find = NULL;
	global_flags = 0;
	return result;
}

void lk_data_remove(void *ptr) {
	linked_list_remove(list, ptr);
}

void lk_data_fini() {
	linked_list_fini(list);
}
