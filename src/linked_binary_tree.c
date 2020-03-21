#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"
#include "linked_binary_tree.h"

static LINKED_NODE *linked_node_find(LINKED_NODE *linked_node, void *key) {
	if (linked_node == NULL) return NULL;
	else if (linked_node->key < key) return linked_node_find(linked_node->right, key);
	else if (linked_node->key > key) return linked_node_find(linked_node->left, key);
	else return linked_node;
}

static LINKED_NODE *create_linked_node(void *key, void *data) {
	LINKED_NODE *linked_node = malloc(sizeof(LINKED_NODE));
	if (linked_node == NULL) {
		fprintf(stderr, "WARNING MALLOC RETURNED NULL\n");
		return NULL;
	}
	linked_node->key = key;
	linked_node->list = linked_list_init();
	linked_list_insert(linked_node->list, data);
	return linked_node;
}

static LINKED_NODE *linked_node_insert(LINKED_NODE *linked_node, void *key, void *data) {
	if (linked_node == NULL) {
		linked_node = create_linked_node(key, data);
	} else if (linked_node->key < key) {
		linked_node->right = linked_node_insert(linked_node->right, key, data);
	} else if (linked_node->key > key) {
		linked_node->left = linked_node_insert(linked_node->left, key, data);
	} else if (linked_node->key == key) {
		linked_list_insert(linked_node->list, data);
	}
	return linked_node;
}

static void linked_node_fini(LINKED_NODE *linked_node, void (*onRemove)(void *)) {
	if (linked_node != NULL) {
		LINKED_LIST *list = linked_node->list;
		if (list) linked_list_fini(list, onRemove);
		linked_node_fini(linked_node->left, onRemove);
		linked_node_fini(linked_node->right, onRemove);
		free(linked_node);
	}
}

LINKED_BINARY_TREE *linked_binary_tree_init() {
	LINKED_BINARY_TREE *tree = malloc(sizeof(LINKED_BINARY_TREE));
	tree->root = NULL;
	/* TODO anything else */
	return tree;
}

void *linked_binary_tree_find(LINKED_BINARY_TREE *tree, void *key) {
	LINKED_NODE *root = tree->root;
	LINKED_NODE *linked_node = linked_node_find(root, key);
	LINKED_LIST *list = linked_node->list;
	return list == NULL ? NULL : list->root;
}

void linked_binary_tree_insert(LINKED_BINARY_TREE *tree, void *key, void *data) {
	LINKED_NODE *root = tree->root;
	tree->root = linked_node_insert(root, key, data);
}

void linked_binary_tree_fini(LINKED_BINARY_TREE *tree, void (*onRemove)(void *)) {
	LINKED_NODE *root = tree->root;
	linked_node_fini(root, onRemove);
}

