#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node {
	void *data;
	struct node *next;
	struct node *prev;
} NODE;

typedef struct linked_list {
	NODE *root;
	NODE *end;
} LINKED_LIST;

LINKED_LIST *linked_list_init();

void linked_list_insert(LINKED_LIST *list, void *data);

void linked_list_iterate(LINKED_LIST *list, void (*consumer)(void *));

void linked_list_reverse_iterate(LINKED_LIST *list, void (*consumer)(void *));

void *linked_list_find(LINKED_LIST *list, int (*finder)(void *));

void linked_list_fini(LINKED_LIST *list, void (*onRemove)(void *));

#endif /* LINKED_LIST_H */
