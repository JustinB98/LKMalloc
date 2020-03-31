#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lk_record.h"

#define MALLOC_RECORD(record) ((record)->sub_record.malloc_record)
#define FREE_RECORD(record) ((record)->sub_record.free_record)

LK_RECORD *lk_create_record(int record_type, u_int flags, void **ptr_passed, LK_METADATA *metadata) {
	LK_RECORD *record = malloc(sizeof(LK_RECORD));
	if (record != NULL) {
		record->record_type = record_type;
		record->retval = 0;
		record->flags = flags;
		if (ptr_passed != NULL) record->ptr_passed = *ptr_passed;
		else record->ptr_passed = NULL;
		record->metadata = *metadata;
		memset(&record->sub_record, 0, sizeof(record->sub_record));
	} else fprintf(stderr, "Could not create record! Returning -ENOMEM\n");
	return record;
}

LK_RECORD *lk_create_malloc_record(u_int flags, void **ptr_passed, LK_METADATA *metadata, u_int size) {
	LK_RECORD *record = lk_create_record(0, flags, ptr_passed, metadata);
	if (record != NULL) {
		lk_malloc_record_set_size(record, size);
	}
	return record;
}

u_int lk_record_get_flags(LK_RECORD *record) {
	return record->flags;
}

int lk_record_get_line_num(LK_RECORD *record) {
	return record->metadata.line_num;
}

char *lk_record_get_file_name(LK_RECORD *record) {
	return record->metadata.file_name;
}

const char *lk_record_get_function_name(LK_RECORD *record) {
	return record->metadata.function_name;
}

size_t lk_record_get_timestamp(LK_RECORD *record) {
	struct timeval *timestamp = &record->metadata.timestamp;
	return timestamp->tv_sec + timestamp->tv_usec;
}

int lk_malloc_record_get_times_freed(LK_RECORD *record) {
	return MALLOC_RECORD(record).times_freed;
}

void lk_malloc_record_increment_times_freed(LK_RECORD *record) {
	++(MALLOC_RECORD(record).times_freed);
}

u_int lk_malloc_record_get_size(LK_RECORD *record) {
	return MALLOC_RECORD(record).size;
}

void lk_malloc_record_set_size(LK_RECORD *record, u_int size) {
	MALLOC_RECORD(record).size = size;
}

void *lk_malloc_record_get_addr_returned(LK_RECORD *record) {
	return MALLOC_RECORD(record).addr_returned;
}

void lk_malloc_record_set_addr_returned(LK_RECORD *record, void *addr) {
	MALLOC_RECORD(record).addr_returned = addr;
}

void *lk_malloc_record_get_malloced_ptr(LK_RECORD *record) {
	return MALLOC_RECORD(record).malloced_ptr;
}

int lk_malloc_record_was_freed_approx(LK_RECORD *record) {
	return MALLOC_RECORD(record).freed_approx;
}

void lk_malloc_record_set_freed_approx(LK_RECORD *record, int was_free_approx) {
	MALLOC_RECORD(record).freed_approx = was_free_approx;
}

void lk_malloc_record_set_malloced_ptr(LK_RECORD *record, void *malloced_ptr) {
	MALLOC_RECORD(record).malloced_ptr = malloced_ptr;
}

void *lk_free_record_get_ptr_requested(LK_RECORD *record) {
	return FREE_RECORD(record).ptr_requested;
}

void lk_free_record_set_ptr_requested(LK_RECORD *record, void *ptr_requested) {
	FREE_RECORD(record).ptr_requested = ptr_requested;
}

void *lk_free_record_get_user_ptr_returned(LK_RECORD *record) {
	return FREE_RECORD(record).user_ptr_returned;
}

void lk_free_record_set_user_ptr_returned(LK_RECORD *record, void *ptr_returned) {
	FREE_RECORD(record).user_ptr_returned = ptr_returned;
}

int lk_free_record_get_times_freed(LK_RECORD *record) {
	return FREE_RECORD(record).times_freed;
}

void lk_free_record_set_times_freed(LK_RECORD *record, int times_freed) {
	FREE_RECORD(record).times_freed = times_freed;
}

int lk_free_record_is_orphaned_free(LK_RECORD *record) {
	return FREE_RECORD(record).orphaned_free;
}

void lk_free_record_set_orphaned_free(LK_RECORD *record, int orphaned_free) {
	FREE_RECORD(record).orphaned_free = orphaned_free;
}

int lk_record_get_record_type(LK_RECORD *record) {
	return record->record_type;
}

void *lk_record_get_ptr_passed(LK_RECORD *record) {
	return record->ptr_passed;
}

int lk_record_get_retval(LK_RECORD *record) {
	return record->retval;
}

void lk_record_set_retval(LK_RECORD *record, int retval) {
	record->retval = retval;
}

int lk_print_malloc_record(LK_RECORD *record, int fd) {
	LK_MALLOC_RECORD *malloc_record = &MALLOC_RECORD(record);
	return dprintf(fd, "%d,%s,%s,%d,%lu,%p,%d,%u,%p\n",
			record->record_type, record->metadata.file_name, record->metadata.function_name,
			record->metadata.line_num, lk_record_get_timestamp(record),
			record->ptr_passed, record->retval, malloc_record->size,
			malloc_record->addr_returned);
}

int lk_print_free_record(LK_RECORD *record, int fd) {
	return dprintf(fd, "%d,%s,%s,%d,%lu,%p,%d,%u\n",
			record->record_type, record->metadata.file_name, record->metadata.function_name,
			record->metadata.line_num, lk_record_get_timestamp(record),
			record->ptr_passed, record->retval, record->flags);
}

