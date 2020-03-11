#ifndef LK_DATA_H
#define LK_DATA_H

typedef struct lk_data {
	void *ptr;
	u_int size;
	int freed;
	u_int flags;
} LK_DATA;

void lk_data_init();

void lk_data_insert(void *ptr, u_int size, u_int flags);

LK_DATA *lk_data_find(void *ptr);

void lk_data_remove(void *ptr);

LK_DATA *lk_data_find(void *ptr);

void lk_data_fini();

#endif /* LK_DATA_H */
