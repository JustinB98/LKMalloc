#include <stdlib.h>
#include <stdio.h>

#include "lk_record.h"

#define MALLOC_RECORD(record) ((record)->sub_record.malloc_record)
#define FREE_RECORD(record) ((record)->sub_record.free_record)

LK_RECORD *lk_record_create(int record_type, u_int flags, void **ptr_passed, LK_METADATA *metadata) {
	LK_RECORD *record = malloc(sizeof(LK_RECORD));
	if (record != NULL) {
		record->record_type = record_type;
		record->flags = flags;
		if (ptr_passed != NULL) record->ptr_passed = *ptr_passed;
		record->metadata = *metadata;
	}
	return record;
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

void lk_malloc_record_set_malloced_ptr(LK_RECORD *record, void *malloced_ptr) {
	MALLOC_RECORD(record).malloced_ptr = malloced_ptr;
}

void *lk_free_record_get_ptr_requested(LK_RECORD *record) {
	return FREE_RECORD(record).ptr_requested;
}

void lk_free_record_set_ptr_requested(LK_RECORD *record, void *ptr_requested) {
	FREE_RECORD(record).ptr_requested = ptr_requested;
}

void *lk_free_record_get_ptr_freed(LK_RECORD *record) {
	return FREE_RECORD(record).ptr_freed;
}

void lk_free_record_set_ptr_freed(LK_RECORD *record, void *ptr_freed) {
	FREE_RECORD(record).ptr_freed = ptr_freed;
}

int lk_free_record_get_times_freed(LK_RECORD *record) {
	return FREE_RECORD(record).times_freed;
}

void lk_free_record_increment_times_freed(LK_RECORD *record) {
	++(FREE_RECORD(record).times_freed);

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

