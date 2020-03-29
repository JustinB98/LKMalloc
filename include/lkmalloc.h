#ifndef LK_MALLOC_H
#define LK_MALLOC_H

#include <stdlib.h>

#define LKM_REG 0x0
#define LKM_INIT 0x1
#define LKM_OVER 0x2
#define LKM_UNDER 0x4
#define LKM_PROT_AFTER 0x8
#define LKM_PROT_BEFORE 0x10

#define LKF_REG 0x0
#define LKF_APPROX 0x1
#define LKF_WARN 0x2
#define LKF_UNKNOWN 0x4
#define LKF_ERROR 0x8

#define LKR_NONE 0x0
#define LKR_SERIOUS 0x1
#define LKR_MATCH 0x2
#define LKR_BAD_FREE 0x4
#define LKR_ORPHAN_FREE 0x8
#define LKR_DOUBLE_FREE 0x10
#define LKR_APPROX 0x20

/*
 * Mallocs a specified amount of memory and fills pointer passed with that address
 * @param size	Size to malloc
 * @param ptr	Double pointer that pointers to the area to store the malloced pointer to
 * @param flags	Flags specify options
 * @return		0 if success
 *				-EINVAL if error
 */
#define lkmalloc(size, ptr, flags) __lkmalloc_internal((size), (ptr), (flags), __FILE__, __func__, __LINE__)

/*
 * Frees pointer from lkmalloc
 * @param ptr	Pointer that points to address to be freed
 * @param flags	Flags to change the behavior of lkfree
 * @return		0 for success
 *				-EINVAL if invalid arugments
 *				-ENOENT if pointer could not be found
 */
#define lkfree(ptr, flags) __lkfree_internal((ptr), (flags), __FILE__, __func__, __LINE__)

/*
 * Prints report to specified file descriptor
 * @param fd	File descriptor to print report to
 * @param flags	Options to change what gets printed
 * @return		number of records printed if success (greater than or equal to 0)
 *				-EINVAL for invalid arguments
 * 				-errno if error (usually relating to printing)
 */
#define lkreport(fd, flags) __lkreport_internal((fd), (flags))

/* INTERNAL CALL DO NOT INVOKE DIRECTLY */
int __lkmalloc_internal(u_int size, void **ptr, u_int flags, char *file, const char *func, int line);

/* INTERNAL CALL DO NOT INVOKE DIRECTLY */
int __lkfree_internal(void **ptr, u_int flags, char *file, const char *func, int line);

/* INTERNAL CALL DO NOT INVOKE DIRECTLY */
int __lkreport_internal(int fd, u_int flags);

#endif /* LK_MALLOC_H */
