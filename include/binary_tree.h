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

/**
 * Initializes a tree using malloc
 * @returns	tree to pass into the other functions in this module.
 *			NULL if malloc failed
 */
BINARY_TREE *binary_tree_init();

/**
 * Inserts key and value pair into binary tree
 * @param tree	Tree to insert key and data into
 * @param key	Key to search for later
 * @param data	Data to insert
 */
void binary_tree_insert(BINARY_TREE *tree, void *key, void *data);

/**
 * Replaces value at key in binary there. Inserts it if it doesn't exist
 * @param tree	Tree to replace in
 * @param key	Key to search for and replace existing data with paramter "data"
 * @param data	Data to replace existing data with
 */
void binary_tree_replace(BINARY_TREE *tree, void *key, void *data);

/**
 * Finds data in tree with given key
 * @param tree	Tree to search key
 * @param key	Key to search for
 * @return		data inserted previous from binary_tree_insert or binary_tree_replace
 * 				NULL if couldn't be found
 */
void *binary_tree_find(BINARY_TREE *tree, void *key);

/**
 * Finds data in tree with given function
 * @param tree		Tree to search
 * @param key		Key as a reference of where to go and look (left or right)
 * @param ref		Reference to pass back in the finder function (to avoid globals)
 * @param finder	Function that takes in the reference passed and current data.
 *					Returns 0 if not the right data, non zero if it is
 * @return			Data found or NULL if finder did not find anything to its liking.
 */
void *binary_tree_find_with_function(BINARY_TREE *tree, void *key, void *ref, int (*finder)(void *, void *));

/**
 * Removes key in tree
 * @param tree	Tree to search for and remove key and its data
 * @param key	Key to search for
 */
void binary_tree_remove(BINARY_TREE *tree, void *key);

/**
 * Frees all nodes in the tree and passes data into onRemove to free the inner data
 * @param tree		Tree to free
 * @param onRemove	Function to help remove the data. Ignored if NULL
 */
void binary_tree_fini(BINARY_TREE *tree, void (*onRemove)(void *));

#endif /* BINARY_TREE_H */
