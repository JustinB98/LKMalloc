#include <stdlib.h>
#include <stdio.h>

#include "binary_tree.h"

static NODE *node_find(NODE *node, void *key) {
	if (node == NULL) return NULL;
	else if (node->key < key) return node_find(node->right, key);
	else if (node->key > key) return node_find(node->left, key);
	else return node;
}

static NODE *create_node(void *key, void *data) {
	NODE *node = malloc(sizeof(NODE));
	if (node == NULL) {
		fprintf(stderr, "WARNING MALLOC RETURNED NULL\n");
		return NULL;
	}
	node->key = key;
	node->data = data;
	return node;
}

static NODE *node_insert(NODE *node, void *key, void *data) {
	if (node == NULL) {
		node = create_node(key, data);
	} else if (node->key < key) {
		node->right = node_insert(node->right, key, data);
	} else if (node->key > key) {
		node->left = node_insert(node->left, key, data);
	}
	return node;
}

static NODE *get_and_set_up_successor(NODE *parent, NODE *node) {
	if (node->left == NULL) {
		parent->left = NULL;
		return node;
	} else {
		return get_and_set_up_successor(node, node->left);
	}
}

static NODE *free_node(NODE *parent, NODE *node) {
	NODE *new_node = NULL;
	if (node->left == NULL && node->right == NULL) {
		return NULL;
	} else if (node->left == NULL) {
		new_node = node->right;
	} else if (node->right == NULL) {
		new_node = node->left;
	} else {
		new_node = get_and_set_up_successor(parent, node);	
	}
	free(node);
	return new_node;
}

static NODE *node_remove(NODE *parent, NODE *node, void *key) {
	if (node == NULL) {
		return NULL;
	} else if (node->key < key) {
		node->right = node_remove(node, node->right, key);
		return node;
	} else if (node->key > key) {
		node->left = node_remove(node, node->left, key);
		return node;
	} else {
		return free_node(parent, node);
	}
}

static void node_fini(NODE *node, void (*onRemove)(void *)) {
	if (node != NULL) {
		if (onRemove) onRemove(node->data);
		node_fini(node->left, onRemove);
		node_fini(node->right, onRemove);
		free(node);
	}
}

BINARY_TREE *binary_tree_init() {
	BINARY_TREE *tree = malloc(sizeof(BINARY_TREE));
	tree->root = NULL;
	/* TODO anything else */
	return tree;
}

void *binary_tree_find(BINARY_TREE *tree, void *key) {
	NODE *root = tree->root;
	NODE *node = node_find(root, key);
	return node == NULL ? NULL : node->data;
}

void binary_tree_insert(BINARY_TREE *tree, void *key, void *data) {
	NODE *root = tree->root;
	tree->root = node_insert(root, key, data);
}

void binary_tree_remove(BINARY_TREE *tree, void *key) {
	NODE *root = tree->root;
	node_remove(NULL, root, key);
}

void binary_tree_fini(BINARY_TREE *tree, void (*onRemove)(void *)) {
	NODE *root = tree->root;
	node_fini(root, onRemove);
}

