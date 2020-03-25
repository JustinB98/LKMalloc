#ifndef LK_DATA_H
#define LK_DATA_H

void lk_data_init();

void lk_data_insert_failed_record(LK_RECORD *record);

/**
 * Will insert malloc record into pending list
 */
void lk_data_insert_malloc_record(void *ptr, LK_RECORD *malloc_record);

/**
 * Will attempt to look for a malloc record in the active heap
 * If it can't, then the completed records need to be searched
 */
LK_RECORD *lk_data_insert_free_record(void *ptr, LK_RECORD *free_record, int (*finder)(void *, void *));

int lk_data_iterate_through_all_records(int fd, u_int flags, int (*consumer)(int, u_int, void *));

void lk_data_fini();

#endif /* LK_DATA_H */
