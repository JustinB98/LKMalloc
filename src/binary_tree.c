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
	node->left = NULL;
	node->right = NULL;
	return node;
}

static NODE *node_replace(NODE *node, void *key, void *data) {
	if (node == NULL) {
		node = create_node(key, data);
	} else if (node->key == key) {
		node->data = data;
	} else if (node->key < key) {
		node->right = node_replace(node->right, key, data);
	} else if (node->key > key) {
		node->left = node_replace(node->left, key, data);
	}
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
		parent->left = node->right;
		return node;
	} else {
		return get_and_set_up_successor(node, node->left);
	}
}

static NODE *free_node(NODE *parent, NODE *node) {
	NODE *new_node = NULL;
	if (node->left == NULL && node->right == NULL) {
		new_node = NULL;
	} else if (node->left == NULL) {
		new_node = node->right;
	} else if (node->right == NULL) {
		new_node = node->left;
	} else {
		new_node = get_and_set_up_successor(parent, node);	
		new_node->left = node->left;
		new_node->right = node->right;
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

static void *node_find_with_function(NODE *node, void *key, void *ref, int (*finder)(void *, void *)) {
	if (node == NULL) return NULL;
	else if (finder(ref, node->data)) return node->data;
	else if (node->key > key) return node_find_with_function(node->left, key, ref, finder);
	else if (node->key < key) return node_find_with_function(node->right, key, ref, finder);
	else return node->data;
}

BINARY_TREE *binary_tree_init() {
	BINARY_TREE *tree = malloc(sizeof(BINARY_TREE));
	tree->root = NULL;
	return tree;
}

void *binary_tree_find(BINARY_TREE *tree, void *key) {
	NODE *root = tree->root;
	NODE *node = node_find(root, key);
	return node == NULL ? NULL : node->data;
}

void *binary_tree_find_with_function(BINARY_TREE *tree, void *key, void *ref, int (*finder)(void *, void *)) {
	NODE *root = tree->root;
	return node_find_with_function(root, key, ref, finder);
}

void binary_tree_insert(BINARY_TREE *tree, void *key, void *data) {
	NODE *root = tree->root;
	tree->root = node_insert(root, key, data);
}

void binary_tree_replace(BINARY_TREE *tree, void *key, void *data) {
	NODE *root = tree->root;
	tree->root = node_replace(root, key, data);
}

void binary_tree_remove(BINARY_TREE *tree, void *key) {
	NODE *root = tree->root;
	tree->root = node_remove(NULL, root, key);
}

void binary_tree_fini(BINARY_TREE *tree, void (*onRemove)(void *)) {
	NODE *root = tree->root;
	node_fini(root, onRemove);
	free(tree);
}

