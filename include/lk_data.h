#ifndef LK_DATA_H
#define LK_DATA_H

typedef struct lk_data {
	void *ptr;
	char *file;
	const char *func;
	u_int size, flags;
	int freed, line;
} LK_DATA;

void lk_data_init();

void lk_data_insert(void *ptr, u_int size, u_int flags, char *file, const char *func, int line);

LK_DATA *lk_data_find(void *ptr, u_int flags);

void lk_data_remove(void *ptr);

void lk_data_fini();

#endif /* LK_DATA_H */
