#ifndef BINARY_TREE_H
#define BINARY_TREE_H

typedef struct node {
	struct node *left;
	struct node *right;
	void *key;
	void *data;
} NODE;

typedef struct binary_tree {
	NODE *root;
} BINARY_TREE;

BINARY_TREE *binary_tree_init();

void binary_tree_insert(BINARY_TREE *tree, void *key, void *data);

void *binary_tree_find(BINARY_TREE *tree, void *key);

void binary_tree_remove(BINARY_TREE *tree, void *key);

void binary_tree_fini(BINARY_TREE *tree, void (*onRemove)(void *));

#endif /* BINARY_TREE_H */
