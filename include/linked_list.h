#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct linked_node {
	void *data;
	struct linked_node *next;
	struct linked_node *prev;
} LINKED_NODE;

typedef struct linked_list {
	LINKED_NODE *root;
	LINKED_NODE *end;
} LINKED_LIST;

/**
 * Initializes linked list for use
 * @return	Linked list malloced
 *			NULL if malloc failed
 */
LINKED_LIST *linked_list_init();

/**
 * Inserts data into linked list
 * @param list	list to insert data into
 * @param data	data to be inserted into list
 */
void linked_list_insert(LINKED_LIST *list, void *data);

/**
 * Iterates through linked list in order and passing current data into
 * consumer function
 * @param list		List to iterate over
 * @param consumer	Function that takes the data and does something with it
 */
void linked_list_iterate(LINKED_LIST *list, void (*consumer)(void *));

/**
 * Iterates through linked list in reverse order and passing current data into
 * consumer function
 * @param list		List to iterate over
 * @param consumer	Function that takes the data and does something with it
 */
void linked_list_reverse_iterate(LINKED_LIST *list, void (*consumer)(void *));

/**
 * Uses function to find data in list
 * @param list		List to search in
 * @param finder	Function that takes current data
 * 					Returns 0 if incorrect data, 1 if right data
 * @return			Data to finder's liking
 */
void *linked_list_find(LINKED_LIST *list, int (*finder)(void *));

/**
 * Specialized function for lk libary. Iterates through list and passes back arguments into consumer function
 * Keeps track of how many times consumer returned a positive number
 * @param list		list to iterate over
 * @param fd		argument to pass back into consumer
 * @param flags		argument to pass back into consumer
 * @param consumer	Function that takes in fd and flags passed in as well as current data
 * 					Returns -1 if error and process should be stopped
 *					Returns 0 if counter shouldn't be incremented
 *					Returns positive number if counter should be incremented
 * @return			Times consumer returned with positive number
 *					-1 if consumer returned a negative number
 */
int linked_list_iterate_with_count(LINKED_LIST *list, int fd, u_int flags, int (*consumer)(int, u_int, void *));

/**
 * Frees list and all data (except the ones that were passed in)
 * @param list		List to free
 * @param onRemove	Function on how to free passed in data
 */
void linked_list_fini(LINKED_LIST *list, void (*onRemove)(void *));

#endif /* LINKED_LIST_H */
