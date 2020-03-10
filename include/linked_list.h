#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node {
	struct node *next;
	struct node *prev;
	void *data;
	void *key;
} NODE;

typedef struct linked_list {
	NODE *root;
	void (*onRemove)(void *);
} LINKED_LIST;

LINKED_LIST *linked_list_init(void (*onRemove)(void *));

void linked_list_insert(LINKED_LIST *list, void *key, void *data);

void linked_list_remove(LINKED_LIST *list, void *key);

void *linked_list_find(LINKED_LIST *list, void *key);

void linked_list_fini(LINKED_LIST *list);

#endif
