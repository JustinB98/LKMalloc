#ifndef LK_LIB_H
#define LK_LIB_H

#define LKM_REG 0x0
#define LKM_INIT 0x1
#define LKM_OVER 0x2
#define LKM_UNDER 0x4
#ifdef EXTRA_CREDIT
#define LKM_PROT_BEFORE 0x8
#define LKM_PROT_AFTER 0x10
#endif

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

#define lkmalloc(size, ptr, flags) __lkmalloc_internal((size), (ptr), (flags), __FILE__, __func__, __LINE__)
#define lkfree(ptr, flags) __lkfree_internal((ptr), (flags), __FILE__, __func__, __LINE__)
#define lkreport(fd, flags) __lkreport_internal((fd), (flags))

int __lkmalloc_internal(u_int size, void **ptr, u_int flags, char *file, const char *func, int line);

int __lkfree_internal(void **ptr, u_int flags, char *file, const char *func, int line);

int __lkreport_internal(int fd, u_int flags);

#endif /* LK_LIB_H */
