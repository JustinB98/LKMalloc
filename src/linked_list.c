#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"

LINKED_LIST *linked_list_init() {
	LINKED_LIST *list = malloc(sizeof(LINKED_LIST));
	if (list != NULL) {
		list->root = NULL;
		list->end = NULL;
	}
	return list;
}

static LINKED_NODE *create_linked_node(void *data) {
	LINKED_NODE *linked_node = malloc(sizeof(LINKED_NODE));
	if (linked_node == NULL) {
		fprintf(stderr, "WARNING, LINKED_NODE COULD NOT BE CREATED");
		return NULL;
	}
	linked_node->data = data;
	linked_node->next = NULL;
	linked_node->prev = NULL;
	return linked_node;
}

void linked_list_insert(LINKED_LIST *list, void *data) {
	LINKED_NODE *root = list->root;
	LINKED_NODE *new_root = create_linked_node(data);
	if (root != NULL) {
		root->prev = new_root;
	} else {
		list->end = new_root;
	}
	new_root->next = root;
	list->root = new_root;
}

void linked_list_iterate(LINKED_LIST *list, void (*consumer)(void *)) {
	LINKED_NODE *current = list->end;
	while (current != NULL) {
		consumer(current->data);
		current = current->prev;
	}
}

void linked_list_reverse_iterate(LINKED_LIST *list, void (*consumer)(void *)) {
	LINKED_NODE *current = list->root;
	while (current != NULL) {
		consumer(current->data);
		current = current->next;
	}
}

void *linked_list_find(LINKED_LIST *list, int (*finder)(void *)) {
	LINKED_NODE *current = list->root;
	while (current != NULL) {
		if (finder(current->data)) return current->data;
		current = current->next;
	}
	return NULL;
}

int linked_list_iterate_with_count(LINKED_LIST *list, int fd, u_int flags, int (*consumer)(int, u_int, void *)) {
	LINKED_NODE *current = list->end;
	int count = 0;
	while (current != NULL) {
		int result = consumer(fd, flags, current->data);
		if (result < 0) return -1;
		else if (result > 0) ++count;
		current = current->prev;
	}
	return count;
}

void linked_list_fini(LINKED_LIST *list, void (*onRemove)(void *)) {
	LINKED_NODE *current = list->root;
	while (current != NULL) {
		onRemove(current->data);
		LINKED_NODE *tmp = current;
		current = current->next;
		free(tmp);
	}
	free(list);
}
