#ifndef LINKED_BINARY_TREE_H
#define LINKED_BINARY_TREE_H

typedef struct linked_node {
	struct linked_node *left;
	struct linked_node *right;
	void *key;
	LINKED_LIST *list;
} LINKED_NODE;

typedef struct linked_binary_tree {
	LINKED_NODE *root;
} LINKED_BINARY_TREE;

LINKED_BINARY_TREE *linked_binary_tree_init();

void linked_binary_tree_insert(LINKED_BINARY_TREE *tree, void *key, void *data);

void *linked_binary_tree_find(LINKED_BINARY_TREE *tree, void *key);

void *linked_binary_tree_find_with_function(LINKED_BINARY_TREE *tree, void *key, void *ref,
											int (*finder)(void *, void *));

void linked_binary_tree_remove(LINKED_BINARY_TREE *tree, void *key);

void linked_binary_tree_fini(LINKED_BINARY_TREE *tree, void (*onRemove)(void *));

#endif /* LINKED_BINARY_TREE_H */
