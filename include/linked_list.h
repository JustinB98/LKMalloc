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

LINKED_LIST *linked_list_init();

void linked_list_insert(LINKED_LIST *list, void *data);

void linked_list_iterate(LINKED_LIST *list, void (*consumer)(void *));

void linked_list_reverse_iterate(LINKED_LIST *list, void (*consumer)(void *));

void *linked_list_find(LINKED_LIST *list, int (*finder)(void *));

int linked_list_iterate_with_count(LINKED_LIST *list, int fd, u_int flags, int (*consumer)(int, u_int, void *));

void linked_list_fini(LINKED_LIST *list, void (*onRemove)(void *));

#endif /* LINKED_LIST_H */
