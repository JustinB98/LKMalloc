#ifndef LK_DATA_H
#define LK_DATA_H

/**
 * Initializes lk data module
 * This includes the active records tree, completed records tree, and all records list
 */
void lk_data_init();

/**
 * Inserts a failed record into the all records list
 * @param record	Record to insert
 */
void lk_data_insert_failed_record(LK_RECORD *record);

/**
 * Will insert malloc record into pending list
 * @param ptr			Pointer the user got
 * @param malloc_record	record to insert
 */
void lk_data_insert_malloc_record(void *ptr, LK_RECORD *malloc_record);

/**
 * Will attempt to look for a malloc record in the active heap
 * If it can't, then the completed records need to be searched
 * @param ptr			Pointer requested from user
 * @param free_record	Record to insert
 * @param finder		Finder to pass into binary tree and linked list modules
 * @return				Associated malloc record if found in active record tree
 *						NULL otherwise
 */
LK_RECORD *lk_data_insert_free_record(void *ptr, LK_RECORD *free_record, int (*finder)(void *, void *));

/**
 * Iterates through all records and passes all arguments
 * into the linked_list_iterate_with_count function
 * @param fd		file descriptor to print records
 * @param flags		lkreport flags
 * @param consumer	Consumer to pass into linked list module
 * @return			linked_list_iterate_with_count_function result
 */
int lk_data_iterate_through_all_records(int fd, u_int flags, int (*consumer)(int, u_int, void *));

/**
 * Cleans up all the data relating to the lkmalloc library
 */ 
void lk_data_fini();

#endif /* LK_DATA_H */
