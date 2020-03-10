#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

typedef int (*queue_function_t)(void *);

/**
 * Function to determine priority
 * - | Second element has more priority
 * 0 | Equal priority
 * + | First element has more priority
 */
typedef int (*queue_comparator_t)(void *, void *);

typedef struct queue_list {
    void *data;
    struct queue_list *next;
    struct queue_list *prev;
} QUEUE_LIST;

typedef struct priority_queue {
    QUEUE_LIST *queue_list;
    queue_comparator_t comparator;
} PRIORITY_QUEUE;

typedef struct queue_iterator {
    QUEUE_LIST *current;
} QUEUE_IT;

/**
 * Initalizes priority queue given how to prioritize each element
 */
PRIORITY_QUEUE *priority_queue_init(queue_comparator_t);

/**
 * Finializes priority queue and passes the data to a function before freeing
 */
void priority_queue_fini(PRIORITY_QUEUE *, void (*)(void *));

/**
 * Finds element given a function
 */
void *queue_find(PRIORITY_QUEUE *, queue_function_t);

/**
 * Inserts element into queue using the given queue comparator
 * If there are elements that are "equal" in priority,
 * the element will be given less priority (priority will be whoever was inserted first)
 */
void queue_insert(PRIORITY_QUEUE *, void *);

/**
 * Returns the head of the queue
 */
void *queue_head(PRIORITY_QUEUE *);

/**
 * Removes an first element from a queue that the queue function returned one for
 * @return  0 if element was removed, -1 if not
 */
int queue_remove(PRIORITY_QUEUE *, queue_function_t);

/**
 * Removes exact element passed in
 * @return  0 if element was removed, -1 if not
 */
int queue_remove_element(PRIORITY_QUEUE *, void *);

/**
 * Passes each element into the consumer function (mostly for debugging)
 */
void queue_iter(PRIORITY_QUEUE *queue, void (*)(void *));

QUEUE_IT *queue_iterator_init(PRIORITY_QUEUE *);

void queue_iterator_fini(QUEUE_IT *);

/**
 * Returns 1 queue iterator isn't done
 */
int queue_it_has_next(QUEUE_IT *);

/**
 * Returns data and advances iterator
 */
void *queue_it_next(QUEUE_IT *);

#endif