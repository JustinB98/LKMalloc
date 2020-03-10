#include <stdlib.h>
#include "priority_queue.h"

PRIORITY_QUEUE *priority_queue_init(queue_comparator_t comparator) {
    PRIORITY_QUEUE *queue = malloc(sizeof(PRIORITY_QUEUE));
    queue->comparator = comparator;
    queue->queue_list = NULL;
    return queue;
}

void priority_queue_fini(PRIORITY_QUEUE *queue, void (*on_removal)(void *)) {
    QUEUE_LIST *list = queue->queue_list, *prev;
    while (list) {
        prev = list;
        list = list->next;
        if (on_removal) on_removal(prev->data);
        free(prev->data);
        free(prev);
    }
    free(queue);
}

void *queue_find(PRIORITY_QUEUE *queue, queue_function_t finder_func) {
    QUEUE_LIST *list = queue->queue_list;
    while (list) {
        if (finder_func(list->data)) {
            return list->data;
        }
        list = list->next;
    }
    return NULL;
}

void queue_insert(PRIORITY_QUEUE *queue, void *data) {
    QUEUE_LIST *list = queue->queue_list;
    if (list == NULL) {
        queue->queue_list = malloc(sizeof(QUEUE_LIST));
        list = queue->queue_list;
        list->next = NULL;
        list->prev = NULL;
        list->data = data;
        return;
    }
    QUEUE_LIST *prev = NULL;
    while (list) {
        int result = queue->comparator(data, list->data);
        if (result > 0) { /* Current element has more priority, we want to replace them */
            QUEUE_LIST *new_element = malloc(sizeof(QUEUE_LIST));
            if (list->prev == NULL) {
                queue->queue_list = new_element;
                new_element->prev = NULL;
                new_element->next = list;
                new_element->data = data;
            } else {
                list->prev->next = new_element;
                new_element->prev = list->prev;
                list->prev = new_element;
                new_element->next = list;
                new_element->data = data;
            }
            return;
        }
        prev = list;
        list = list->next;
    }
    /* If we made it here, then the element is last in the list (least priority). Prev is original last */
    QUEUE_LIST *new_element = malloc(sizeof(QUEUE_LIST));
    prev->next = new_element;
    new_element->data = data;
    new_element->next = NULL;
    new_element->prev = prev;
}

void *queue_head(PRIORITY_QUEUE *queue) {
    QUEUE_LIST *list = queue->queue_list;
    if (list == NULL) return NULL;
    else return list->data;
}

static void remove_element(PRIORITY_QUEUE *queue, QUEUE_LIST *list) {
    if (list->next == NULL && list->prev == NULL) { /* Only one element */
        free(list->data);
        free(list);
        queue->queue_list = NULL;
    } else if (list->prev == NULL) { /* First element */
        list->next->prev = NULL;
        queue->queue_list = list->next;
        free(list->data);
        free(list);
    } else { /* All others */
        if (list->next != NULL) list->next->prev = list->prev;
        list->prev->next = list->next;
        free(list->data);
        free(list);
    }
}

int queue_remove(PRIORITY_QUEUE *queue, queue_function_t remover_func) {
    QUEUE_LIST *list = queue->queue_list;
    while (list) {
        if (remover_func(list->data)) {
            remove_element(queue, list);
            return 0;
        }
        list = list->next;
    }
    return -1;
}

int queue_remove_element(PRIORITY_QUEUE *queue, void *data) {
    QUEUE_LIST *list = queue->queue_list;
    while (list) {
        if (list->data == data) {
            remove_element(queue, list);
            return 0;
        }
        list = list->next;
    }
    return -1;
}

void queue_iter(PRIORITY_QUEUE *queue, void (*consumer)(void *)) {
    QUEUE_LIST *list = queue->queue_list;
    while (list) {
        consumer(list->data);
        list = list->next;
    }
}

QUEUE_IT *queue_iterator_init(PRIORITY_QUEUE *queue) {
    QUEUE_IT *it = malloc(sizeof(QUEUE_IT));
    it->current = queue->queue_list;
    return it;
}

void queue_iterator_fini(QUEUE_IT *it) {
    free(it);
}

int queue_it_has_next(QUEUE_IT *it) {
    return it->current != NULL;
}

/**
 * Returns data and advances iterator
 */
void *queue_it_next(QUEUE_IT *it) {
    if (it->current == NULL) return NULL;
    void *curr = it->current->data;
    it->current = it->current->next;
    return curr;
}
