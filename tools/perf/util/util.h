/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PERF_UTIL_H
#define __PERF_UTIL_H

#define _BSD_SOURCE 1
/* glibc 2.20 deprecates _BSD_SOURCE in favour of _DEFAULT_SOURCE */
#define _DEFAULT_SOURCE 1

#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <linux/compiler.h>
#include <sys/types.h>
#ifndef __cplusplus
#include <internal/cpumap.h>
#endif

extern const char perf_usage_string[];
extern const char perf_more_info_string[];

extern const char *input_name;

/* This will control if perf_{host,guest} will set attr.exclude_{host,guest}. */
extern bool exclude_GH_default;

extern bool perf_host;
extern bool perf_guest;

/* General helper functions */
void usage(const char *err) __noreturn;
void die(const char *err, ...) __noreturn __printf(1, 2);

struct dirent;
struct strlist;

int mkdir_p(char *path, mode_t mode);
int rm_rf(const char *path);
int rm_rf_perf_data(const char *path);
struct strlist *lsdir(const char *name, bool (*filter)(const char *, struct dirent *));
bool lsdir_no_dot_filter(const char *name, struct dirent *d);

size_t hex_width(u64 v);

int sysctl__max_stack(void);

bool sysctl__nmi_watchdog_enabled(void);

int perf_tip(char **strp, const char *dirpath);

#ifndef HAVE_SCHED_GETCPU_SUPPORT
int sched_getcpu(void);
#endif

#ifndef HAVE_SCANDIRAT_SUPPORT
int scandirat(int dirfd, const char *dirp,
	      struct dirent ***namelist,
	      int (*filter)(const struct dirent *),
	      int (*compar)(const struct dirent **, const struct dirent **));
#endif

extern bool perf_singlethreaded;

void perf_set_singlethreaded(void);
void perf_set_multithreaded(void);

char *perf_exe(char *buf, int len);

#ifndef O_CLOEXEC
#ifdef __sparc__
#define O_CLOEXEC      0x400000
#elif defined(__alpha__) || defined(__hppa__)
#define O_CLOEXEC      010000000
#else
#define O_CLOEXEC      02000000
#endif
#endif

struct perf_debuginfod {
	const char	*urls;
	bool		 set;
};
void perf_debuginfod_setup(struct perf_debuginfod *di);

char *filename_with_chroot(int pid, const char *filename);

int do_realloc_array_as_needed(void **arr, size_t *arr_sz, size_t x,
			       size_t msz, const void *init_val);

#define realloc_array_as_needed(a, n, x, v) ({			\
	typeof(x) __x = (x);					\
	__x >= (n) ?						\
		do_realloc_array_as_needed((void **)&(a),	\
					   &(n),		\
					   __x,			\
					   sizeof(*(a)),	\
					   (const void *)(v)) :	\
		0;						\
	})

static inline bool host_is_bigendian(void)
{
#ifdef __BYTE_ORDER__
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return false;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return true;
#else
#error "Unrecognized __BYTE_ORDER__"
#endif
#else /* !__BYTE_ORDER__ */
	unsigned char str[] = { 0x1, 0x2, 0x3, 0x4, 0x0, 0x0, 0x0, 0x0};
	unsigned int *ptr;

	ptr = (unsigned int *)(void *)str;
	return *ptr == 0x01020304;
#endif
}

#endif /* __PERF_UTIL_H */
