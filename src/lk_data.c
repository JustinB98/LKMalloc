#include <stdlib.h>
#include <stdio.h>

#include "lk_data.h"
#include "linked_list.h"

static LINKED_LIST *list;

static void *current_ptr_to_find;

static int lk_finder(void *data) {
	LK_DATA *lk_data = data;
	return lk_data->ptr == current_ptr_to_find;
}

static void onRemoval(void *data) {
	LK_DATA *lk_data = data;
	free(lk_data->ptr);
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

LK_DATA *lk_data_find(void *ptr) {
	current_ptr_to_find = ptr;
	LK_DATA *result = linked_list_find(list, lk_finder);
	current_ptr_to_find = NULL;
	return result;
}

void lk_data_remove(void *ptr) {
	linked_list_remove(list, ptr);
}

void lk_data_fini() {
	linked_list_fini(list);
}
