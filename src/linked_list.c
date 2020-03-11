#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"

LINKED_LIST *linked_list_init(void (*onRemove)(void *)) {
	LINKED_LIST *list = malloc(sizeof(LINKED_LIST));
	list->root = NULL;
	list->onRemove = onRemove;
	return list;
}

static NODE *create_node(void *key, void *data) {
	NODE *node = malloc(sizeof(NODE));
	if (node == NULL) {
		fprintf(stderr, "WARNING, NODE COULD NOT BE CREATED");
		return NULL;
	}
	node->key = key;
	node->data = data;
	node->next = NULL;
	node->prev = NULL;
	return node;
}


static NODE *node_insert(NODE *node, void *key, void *data) {
	if (node == NULL) {
		return create_node(key, data);
	} else if (node->next == NULL) {
		NODE *new_node = create_node(key, data);
		node->next = new_node;
		new_node->prev = node;
	} else {
		node->next = node_insert(node->next, key, data);
	}
	return node;
}

void remove_node_from_chain(NODE *node, void *key, void (*onRemove)(void *)) {
	NODE *prev = node->prev;
	NODE *next = node->next;
	if (prev != NULL) {
		prev->next = next;
	}
	if (next != NULL) {
		next->prev = prev;
	}
	onRemove(node->data);
	free(node);
}

static NODE *node_remove(NODE *node, void *key, void (*onRemove)(void *)) {
	if (node == NULL) {
		return NULL;
	} else if (node->key == key) {
		NODE *next = node->next;
		remove_node_from_chain(node, key, onRemove);
		return next;
	} else {
		node->next = node_remove(node->next, key, onRemove);
		return node;
	}
}

static void *node_find(NODE *node, int (*finder)(void*)) {
	if (node == NULL) return NULL;
	else if (finder(node->data)) return node->data;
	else return node_find(node->next, finder);
}

static void node_fini(NODE *node, void (*onRemove)(void *)) {
	if (node == NULL) return;
	onRemove(node->data);
	NODE *next = node->next;
	free(node);
	node_fini(next, onRemove);
}

void linked_list_insert(LINKED_LIST *list, void *key, void *data) {
	NODE *root = list->root;
	list->root = node_insert(root, key, data);
}

void linked_list_remove(LINKED_LIST *list, void *key) {
	NODE *root = list->root;
	list->root = node_remove(root, key, list->onRemove);
}

void *linked_list_find(LINKED_LIST *list, int (*finder)(void *)) {
	NODE *root = list->root;
	return node_find(root, finder);
}

void linked_list_fini(LINKED_LIST *list) {
	NODE *root = list->root;
	node_fini(root, list->onRemove);
	free(list);
}
