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

static NODE *create_node(void *data) {
	NODE *node = malloc(sizeof(NODE));
	if (node == NULL) {
		fprintf(stderr, "WARNING, NODE COULD NOT BE CREATED");
		return NULL;
	}
	node->data = data;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

void linked_list_insert(LINKED_LIST *list, void *data) {
	NODE *root = list->root;
	NODE *new_root = create_node(data);
	if (root != NULL) {
		root->prev = new_root;
	} else {
		list->end = new_root;
	}
	new_root->next = root;
	list->root = new_root;
}

void linked_list_iterate(LINKED_LIST *list, void (*consumer)(void *)) {
	NODE *current = list->end;
	while (current != NULL) {
		consumer(current->data);
		current = current->prev;
	}
}

void linked_list_reverse_iterate(LINKED_LIST *list, void (*consumer)(void *)) {
	NODE *current = list->root;
	while (current != NULL) {
		consumer(current->data);
		current = current->next;
	}
}

void *linked_list_find(LINKED_LIST *list, int (*finder)(void *)) {
	NODE *current = list->root;
	while (current != NULL) {
		if (finder(current->data)) return current->data;
		current = current->next;
	}
	return NULL;
}

void linked_list_fini(LINKED_LIST *list, void (*onRemove)(void *)) {
	NODE *current = list->root;
	while (current != NULL) {
		onRemove(current->data);
		NODE *tmp = current;
		current = current->next;
		free(tmp);
	}
	free(list);
}
