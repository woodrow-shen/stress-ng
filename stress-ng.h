/*
 * Copyright (C) 2013-2021 Canonical, Ltd.
 * Copyright (C) 2021-2023 Colin Ian King
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#ifndef STRESS_NG_H
#define STRESS_NG_H

#include "config.h"

#if defined(__ICC) &&		\
    defined(__INTEL_COMPILER)
/* Intel ICC compiler */
#define HAVE_COMPILER_ICC
#elif defined(__PCC__)
/* Portable C compiler */
#define HAVE_COMPILER_PCC
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#elif defined(__TINYC__)
/* Tiny C Compiler */
#define HAVE_COMPILER_TCC
#undef _FORTIFY_SOURCE
#elif defined(__clang__) && 	\
   (defined(__INTEL_CLANG_COMPILER) || defined(__INTEL_LLVM_COMPILER))
/* Intel ICX compiler */
#define HAVE_COMPILER_ICX
#elif defined(__clang__)
/* clang */
#define HAVE_COMPILER_CLANG
#elif defined(__GNUC__)
/* GNU C compiler */
#define HAVE_COMPILER_GCC
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define _ATFILE_SOURCE
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#if !defined(HAVE_COMPILER_PCC) && 	\
    !defined(HAVE_COMPILER_TCC) &&	\
    !defined(_FORTIFY_SOURCE)
#define _FORTIFY_SOURCE 	(2)
#endif

#if !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 	(64)
#endif

/* Some Solaris tool chains only define __sun */
#if defined(__sun) &&	\
    !defined(__sun__)
#define __sun__
#endif

/*
 *  Standard includes, assume we have this as the
 *  minimal standard baseline
 */
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <pwd.h>
#include <sched.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#if defined(HAVE_FEATURES_H)
#include <features.h>
#endif
#if defined(HAVE_LIB_PTHREAD)
#include <pthread.h>
#endif
#if defined(HAVE_STRINGS_H)
#include <strings.h>
#endif
#if defined(HAVE_TERMIOS_H)
#include <termios.h>
#endif
#if defined(HAVE_BSD_STDLIB_H)
#include <bsd/stdlib.h>
#endif
#if defined(HAVE_BSD_STRING_H)
#include <bsd/string.h>
#endif
#if defined(HAVE_BSD_UNISTD_H)
#include <bsd/unistd.h>
#endif

/*
 *  Various sys include files
 */
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#if defined(HAVE_SYS_PARAM_H)
#include <sys/param.h>
#endif
#if defined(HAVE_SYSCALL_H)
#include <sys/syscall.h>
#endif
#if defined(HAVE_SYS_SYSINFO_H)
#include <sys/sysinfo.h>
#if defined(HAVE_COMPILER_GCC) &&	\
    !defined(__GLIBC__)
/* Suppress kernel sysinfo to avoid collision with musl */
#define _LINUX_SYSINFO_H
#endif
#endif
#if defined(HAVE_LINUX_POSIX_TYPES_H)
#include <linux/posix_types.h>
#endif

#include "core-version.h"
#include "core-attribute.h"
#include "core-asm-generic.h"
#include "core-opts.h"
#include "core-parse-opts.h"
#include "core-setting.h"
#include "core-log.h"
#include "core-lock.h"
#include "core-mwc.h"
#include "core-sched.h"
#include "core-shim.h"

#if defined(CHECK_UNEXPECTED) && 	\
    defined(HAVE_PRAGMA) &&		\
    defined(HAVE_COMPILER_GCC)
#define UNEXPECTED_PRAGMA(x) _Pragma (#x)
#define UNEXPECTED_XSTR(x) UNEXPECTED_STR(x)
#define UNEXPECTED_STR(x) # x
#define UNEXPECTED	UNEXPECTED_PRAGMA(message	\
	"unexpected default codepath: line #" 		\
	UNEXPECTED_XSTR(__LINE__) " in " 		\
	UNEXPECTED_XSTR(__FILE__));
#else
#define UNEXPECTED
#endif

#define VOID_RET(type, x)	\
do {				\
	type void_ret = x;	\
				\
	(void)void_ret;		\
} while (0)			\

#define STRESS_BIT_U(shift)		(1U << (shift))
#define STRESS_BIT_UL(shift)		(1UL << (shift))
#define STRESS_BIT_ULL(shift)		(1ULL << (shift))

/* EXIT_SUCCESS, EXIT_FAILURE defined in stdlib.h */
#define EXIT_NOT_SUCCESS		(2)
#define EXIT_NO_RESOURCE		(3)
#define EXIT_NOT_IMPLEMENTED		(4)
#define EXIT_SIGNALED			(5)
#define EXIT_BY_SYS_EXIT		(6)
#define EXIT_METRICS_UNTRUSTWORTHY	(7)

/*
 *  Stressor run states
 */
#define STRESS_STATE_START		(0)
#define STRESS_STATE_INIT		(1)
#define STRESS_STATE_RUN		(2)
#define STRESS_STATE_DEINIT		(3)
#define STRESS_STATE_STOP		(4)
#define STRESS_STATE_EXIT		(5)
#define STRESS_STATE_WAIT		(6)
#define STRESS_STATE_ZOMBIE		(7)

#define	STRESS_INTERRUPTS_MAX		(8)	/* see core_interrupts.c */

/* oomable flags */
#define	STRESS_OOMABLE_NORMAL		(0x00000000)	/* Normal oomability */
#define STRESS_OOMABLE_DROP_CAP		(0x00000001)	/* Drop capabilities */
#define STRESS_OOMABLE_QUIET		(0x00000002)	/* Don't report activity */

/*
 *  Timing units
 */
#define STRESS_NANOSECOND		(1000000000L)
#define STRESS_MICROSECOND		(1000000L)
#define STRESS_MILLISECOND		(1000L)

#define STRESS_DBL_NANOSECOND		(1000000000.0)
#define STRESS_DBL_MICROSECOND		(1000000.0)
#define STRESS_DBL_MILLISECOND		(1000.0)

/*
 * STRESS_ASSERT(test)
 *   throw compile time error if test not true
 */
#define STRESS_CONCAT(a, b) a ## b
#define STRESS_CONCAT_EXPAND(a, b) STRESS_CONCAT(a, b)
#define STRESS_ASSERT(expr) \
	enum { STRESS_CONCAT_EXPAND(STRESS_ASSERT_AT_LINE_, __LINE__) = \
		1 / !!(expr) };

#define STRESS_MINIMUM(a,b) (((a) < (b)) ? (a) : (b))
#define STRESS_MAXIMUM(a,b) (((a) > (b)) ? (a) : (b))

/* NetBSD does not define MAP_ANONYMOUS */
#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif

/* GNU HURD and other systems that don't define PATH_MAX */
#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif

/*
 * making local static fixes clobbering warnings
 */
#define NOCLOBBER static

#define STRESS_TRY_OPEN_OK	  (0)		/* File can be opened */
#define STRESS_TRY_OPEN_FORK_FAIL (1)		/* Try failed, e.g. can't fork */
#define STRESS_TRY_OPEN_WAIT_FAIL (2)		/* Wait on child open failed */
#define STRESS_TRY_OPEN_EXIT_FAIL (3)		/* Can't get _exit() status */
#define STRESS_TRY_OPEN_FAIL	  (4)		/* Can't open file */
#define STRESS_TRY_AGAIN	  (5)		/* Device busy, try again */

#define STRESS_FD_MAX		(65536)		/* Max fds if we can't figure it out */
#define STRESS_PROCS_MAX	(8192)		/* Max number of processes per stressor */

#define ABORT_FAILURES		(5)		/* Number of failures before we abort */

/* debug output bitmasks */
#define PR_ERROR		 STRESS_BIT_ULL(0)	/* Print errors */
#define PR_INFO			 STRESS_BIT_ULL(1)	/* Print info */
#define PR_DEBUG		 STRESS_BIT_ULL(2) 	/* Print debug */
#define PR_FAIL			 STRESS_BIT_ULL(3) 	/* Print test failure message */
#define PR_WARN			 STRESS_BIT_ULL(4)	/* Print warning */
#define PR_METRICS		 STRESS_BIT_ULL(5)	/* Print metrics */
#define PR_ALL			 (PR_ERROR | PR_INFO | PR_DEBUG | PR_FAIL | PR_WARN | PR_METRICS)

/* Option bit masks, stats from the next PR_ option onwards */
#define OPT_FLAGS_METRICS	 STRESS_BIT_ULL(6)	/* --metrics, Dump metrics at end */
#define OPT_FLAGS_RANDOM	 STRESS_BIT_ULL(7)	/* --random, Randomize */
#define OPT_FLAGS_SET		 STRESS_BIT_ULL(8)	/* Set if user specifies stress procs */
#define OPT_FLAGS_KEEP_NAME	 STRESS_BIT_ULL(9)	/* --keep-name, Keep stress names to stress-ng */
#define OPT_FLAGS_METRICS_BRIEF	 STRESS_BIT_ULL(10)	/* --metrics-brief, dump brief metrics */
#define OPT_FLAGS_VERIFY	 STRESS_BIT_ULL(11)	/* --verify, verify mode */
#define OPT_FLAGS_MMAP_MADVISE	 STRESS_BIT_ULL(12)	/* --no-madvise, disable random madvise settings */
#define OPT_FLAGS_MMAP_MINCORE	 STRESS_BIT_ULL(13)	/* --page-in, mincore force pages into mem */
#define OPT_FLAGS_TIMES		 STRESS_BIT_ULL(14)	/* --times, user/system time summary */
#define OPT_FLAGS_MINIMIZE	 STRESS_BIT_ULL(15)	/* --minimize, Minimize */
#define OPT_FLAGS_MAXIMIZE	 STRESS_BIT_ULL(16)	/* --maximize Maximize */
#define OPT_FLAGS_SYSLOG	 STRESS_BIT_ULL(17)	/* --syslog, log test progress to syslog */
#define OPT_FLAGS_AGGRESSIVE	 STRESS_BIT_ULL(18)	/* --aggressive, aggressive mode enabled */
#define OPT_FLAGS_ALL		 STRESS_BIT_ULL(19)	/* --all mode */
#define OPT_FLAGS_SEQUENTIAL	 STRESS_BIT_ULL(20)	/* --sequential mode */
#define OPT_FLAGS_PERF_STATS	 STRESS_BIT_ULL(21)	/* --perf stats mode */
#define OPT_FLAGS_LOG_BRIEF	 STRESS_BIT_ULL(22)	/* --log-brief */
#define OPT_FLAGS_THERMAL_ZONES  STRESS_BIT_ULL(23)	/* --tz thermal zones */
#define OPT_FLAGS_SOCKET_NODELAY STRESS_BIT_ULL(24)	/* --sock-nodelay */
#define OPT_FLAGS_IGNITE_CPU	 STRESS_BIT_ULL(25)	/* --cpu-ignite */
#define OPT_FLAGS_PATHOLOGICAL	 STRESS_BIT_ULL(26)	/* --pathological */
#define OPT_FLAGS_NO_RAND_SEED	 STRESS_BIT_ULL(27)	/* --no-rand-seed */
#define OPT_FLAGS_THRASH	 STRESS_BIT_ULL(28)	/* --thrash */
#define OPT_FLAGS_OOMABLE	 STRESS_BIT_ULL(29)	/* --oomable */
#define OPT_FLAGS_ABORT		 STRESS_BIT_ULL(30)	/* --abort */
#define OPT_FLAGS_TIMESTAMP	 STRESS_BIT_ULL(31)	/* --timestamp */
#define OPT_FLAGS_DEADLINE_GRUB  STRESS_BIT_ULL(32)	/* --sched-reclaim */
#define OPT_FLAGS_FTRACE	 STRESS_BIT_ULL(33)	/* --ftrace */
#define OPT_FLAGS_SEED		 STRESS_BIT_ULL(34)	/* --seed */
#define OPT_FLAGS_SKIP_SILENT	 STRESS_BIT_ULL(35)	/* --skip-silent */
#define OPT_FLAGS_SMART		 STRESS_BIT_ULL(36)	/* --smart */
#define OPT_FLAGS_NO_OOM_ADJUST	 STRESS_BIT_ULL(37)	/* --no-oom-adjust */
#define OPT_FLAGS_KEEP_FILES	 STRESS_BIT_ULL(38)	/* --keep-files */
#define OPT_FLAGS_STDERR	 STRESS_BIT_ULL(39)	/* --stderr */
#define OPT_FLAGS_STDOUT	 STRESS_BIT_ULL(40)	/* --stdout */
#define OPT_FLAGS_KLOG_CHECK	 STRESS_BIT_ULL(41)	/* --klog-check */
#define OPT_FLAGS_DRY_RUN	 STRESS_BIT_ULL(42)	/* --dry-run, don't actually run */
#define OPT_FLAGS_OOM_AVOID	 STRESS_BIT_ULL(43)	/* --oom-avoid */
#define OPT_FLAGS_TZ_INFO	 STRESS_BIT_ULL(44)	/* --tz, enable thermal zone info */
#define OPT_FLAGS_LOG_LOCKLESS	 STRESS_BIT_ULL(45)	/* --log-lockless */
#define OPT_FLAGS_SN		 STRESS_BIT_ULL(46)	/* --sn scientific notation */
#define OPT_FLAGS_CHANGE_CPU	 STRESS_BIT_ULL(47)	/* --change-cpu */
#define OPT_FLAGS_KSM		 STRESS_BIT_ULL(48)	/* --ksm */
#define OPT_FLAGS_SETTINGS	 STRESS_BIT_ULL(49)	/* --settings */
#define OPT_FLAGS_WITH		 STRESS_BIT_ULL(50)	/* --with list */
#define OPT_FLAGS_PERMUTE	 STRESS_BIT_ULL(51)	/* --permute N */

#define OPT_FLAGS_MINMAX_MASK		\
	(OPT_FLAGS_MINIMIZE | OPT_FLAGS_MAXIMIZE)

/* Aggressive mode flags */
#define OPT_FLAGS_AGGRESSIVE_MASK 	\
	(OPT_FLAGS_MMAP_MADVISE |	\
	 OPT_FLAGS_MMAP_MINCORE |	\
	 OPT_FLAGS_AGGRESSIVE |		\
	 OPT_FLAGS_IGNITE_CPU)

/* Stressor classes */
#define CLASS_CPU		STRESS_BIT_UL(0)	/* CPU only */
#define CLASS_MEMORY		STRESS_BIT_UL(1)	/* Memory thrashers */
#define CLASS_CPU_CACHE		STRESS_BIT_UL(2)	/* CPU cache */
#define CLASS_IO		STRESS_BIT_UL(3)	/* I/O read/writes etc */
#define CLASS_NETWORK		STRESS_BIT_UL(4)	/* Network, sockets, etc */
#define CLASS_SCHEDULER		STRESS_BIT_UL(5)	/* Scheduling */
#define CLASS_VM		STRESS_BIT_UL(6)	/* VM stress, big memory, swapping */
#define CLASS_INTERRUPT		STRESS_BIT_UL(7)	/* interrupt floods */
#define CLASS_OS		STRESS_BIT_UL(8)	/* generic OS tests */
#define CLASS_PIPE_IO		STRESS_BIT_UL(9)	/* pipe I/O */
#define CLASS_FILESYSTEM	STRESS_BIT_UL(10)	/* file system */
#define CLASS_DEV		STRESS_BIT_UL(11)	/* device (null, zero, etc) */
#define CLASS_SECURITY		STRESS_BIT_UL(12)	/* security APIs */
#define CLASS_PATHOLOGICAL	STRESS_BIT_UL(13)	/* can hang a machine */
#define CLASS_GPU		STRESS_BIT_UL(14)	/* GPU */

/* Help information for options */
typedef struct {
	const char *opt_s;		/* short option */
	const char *opt_l;		/* long option */
	const char *description;	/* description */
} stress_help_t;

typedef struct {
	uint64_t counter;		/* bogo-op counter */
	bool counter_ready;		/* ready flag */
	bool run_ok;			/* stressor run w/o issues */
	bool force_killed;		/* true if sent SIGKILL */
} stress_counter_info_t;

/*
 *  Per ELISA request, we have a duplicated counter
 *  and run_ok flag in a different shared memory region
 *  so we can sanity check these just in case the stats
 *  have got corrupted.
 */
typedef struct {
	struct {
		stress_counter_info_t ci; /* Copy of stats counter info ci */
		uint8_t	reserved[7];	/* Padding */
	} data;
	uint32_t hash;			/* Hash of data */
} stress_checksum_t;

/*
 *  Scratch space to store computed values to ensure
 *  compiler does not compile away calculations
 */
typedef union {
	volatile uint8_t	uint8_val;
	volatile uint16_t	uint16_val;
	volatile uint32_t	uint32_val;
	volatile uint64_t	uint64_val;
#if defined(HAVE_INT128_T)
	volatile __uint128_t	uint128_val;
#endif
	volatile float		float_val;
	volatile double		double_val;
	volatile long double	long_double_val;
	volatile void 		*void_ptr_val;
} stress_put_val_t;

typedef uint32_t stress_class_t;

typedef struct {
	void *page_none;		/* mmap'd PROT_NONE page */
	void *page_ro;			/* mmap'd PROT_RO page */
	void *page_wo;			/* mmap'd PROT_WO page */
} stress_mapped_t;

#define STRESS_MISC_METRICS_MAX	(40)

typedef struct {
	void *lock;			/* optional lock */
	double	duration;		/* time per op */
	double	count;			/* number of ops */
	volatile double	t_start;	/* optional start time */
} stress_metrics_t;

typedef struct {
	char *description;		/* description of metric */
	double value;			/* value of metric */
} stress_metrics_data_t;

/* stressor args */
typedef struct {
	stress_counter_info_t *ci;	/* counter info struct */
	const char *name;		/* stressor name */
	uint64_t max_ops;		/* max number of bogo ops */
	const uint32_t instance;	/* stressor instance # */
	const uint32_t num_instances;	/* number of instances */
	pid_t pid;			/* stressor pid */
	size_t page_size;		/* page size */
	double time_end;		/* when to end */
	stress_mapped_t *mapped;	/* mmap'd pages, addr of g_shared mapped */
	stress_metrics_data_t *metrics;	/* misc per stressor metrics */
	const struct stressor_info *info; /* stressor info */
} stress_args_t;

typedef struct {
	const int opt;			/* optarg option*/
	int (*opt_set_func)(const char *opt); /* function to set it */
} stress_opt_set_func_t;

typedef enum {
	VERIFY_NONE	= 0x00,		/* no verification */
	VERIFY_OPTIONAL = 0x01,		/* --verify can enable verification */
	VERIFY_ALWAYS   = 0x02,		/* verification always enabled */
} stress_verify_t;

/* stressor information */
typedef struct stressor_info {
	int (*stressor)(const stress_args_t *args);	/* stressor function */
	int (*supported)(const char *name);	/* return 0 = supported, -1, not */
	void (*init)(void);		/* stressor init, NULL = ignore */
	void (*deinit)(void);		/* stressor de-init, NULL = ignore */
	void (*set_default)(void);	/* default set-up */
	void (*set_limit)(uint64_t max);/* set limits */
	const stress_opt_set_func_t *opt_set_funcs;	/* option functions */
	const stress_help_t *help;	/* stressor help options */
	const stress_class_t class;	/* stressor class */
	const stress_verify_t verify;	/* verification mode */
	const char *unimplemented_reason;	/* unsupported reason message */
} stressor_info_t;

/* gcc 4.7 and later support vector ops */
#if defined(HAVE_COMPILER_GCC) &&	\
    NEED_GNUC(4, 7, 0)
#define STRESS_VECTOR	(1)
#endif

/* restrict keyword */
#if defined(HAVE___RESTRICT)
#define RESTRICT __restrict
#else
#define RESTRICT
#endif

/* optimisation on branching */
#if defined(HAVE_BUILTIN_EXPECT)
#define LIKELY(x)	__builtin_expect((x),1)
#define UNLIKELY(x)	__builtin_expect((x),0)
#else
#define LIKELY(x)	(x)
#define UNLIKELY(x)	(x)
#endif

/* use syscall if we can, fallback to vfork otherwise */
#define shim_vfork()		g_shared->vfork()

extern const char stress_config[];

/* Printing/Logging helpers */
extern void pr_yaml_runinfo(FILE *fp);
extern void pr_runinfo(void);

/* Memory size constants */
#define KB			(1ULL << 10)
#define	MB			(1ULL << 20)
#define GB			(1ULL << 30)
#define TB			(1ULL << 40)
#define PB			(1ULL << 50)
#define EB			(1ULL << 60)

#define ONE_BILLIONTH		(1.0E-9)
#define ONE_MILLIONTH		(1.0E-6)
#define ONE_THOUSANDTH		(1.0E-3)

#define STACK_ALIGNMENT		(64)	/* thread stacks align to 64 bytes */

#define MIN_OPS			(1ULL)
#define MAX_OPS			(100000000ULL)
#define MAX_32			(0xffffffffUL)
#define MAX_48			(0xffffffffffffULL)
#define MAX_64			(0xffffffffffffffffULL)

/* Maximum memory limits, 256TB for 64 bit is good enough for 2017 */
#if UINTPTR_MAX == MAX_32
#define MAX_MEM_LIMIT		(MAX_32)
#else
#define MAX_MEM_LIMIT		(MAX_48)
#endif

#define MAX_FILE_LIMIT		((1ULL << ((sizeof(off_t) * 8) - 1)) - 1)
/*
 * --maximize files must not be so big that we fill up
 * a disk, so make them either the MAX_FILE_FILE_LIMIT for
 * systems with small off_t or 4GB for large off_t systems
 */
#define MAXIMIZED_FILE_SIZE	((sizeof(off_t) < 8) ? MAX_FILE_LIMIT : MAX_32)

/* Stressor defaults */

#define MIN_SEQUENTIAL		(0)
#define MAX_SEQUENTIAL		(1000000)
#define DEFAULT_SEQUENTIAL	(0)	/* Disabled */
#define DEFAULT_PARALLEL	(0)	/* Disabled */

#define TIMEOUT_NOT_SET		(~0ULL)
#define DEFAULT_TIMEOUT		(60 * 60 * 24)
#define DEFAULT_BACKOFF		(0)
#define DEFAULT_LINKS		(8192)
#define DEFAULT_DIRS		(8192)

#define MEM_CACHE_SIZE		(65536 * 32)
#define DEFAULT_CACHE_LEVEL     (3)
#define UNDEFINED		(-1)

#define PAGE_MAPPED		(0x01)
#define PAGE_MAPPED_FAIL	(0x02)

#if defined(HAVE_COMPILER_GCC) || defined(HAVE_COMPILER_CLANG)
#define TYPEOF_CAST(a)	(typeof(a))
#else
#define	TYPEOF_CAST(a)
#endif

/* Generic bit setting on an array macros */
#define STRESS_NBITS(a)		(sizeof(a[0]) * 8)
#define STRESS_GETBIT(a, i)	(a[i / STRESS_NBITS(a)] & \
				 (TYPEOF_CAST(a[0])1U << (i & (STRESS_NBITS(a)-1))))
#define STRESS_CLRBIT(a, i)	(a[i / STRESS_NBITS(a)] &= \
				 ~(TYPEOF_CAST(a[0])1U << (i & (STRESS_NBITS(a)-1))))
#define STRESS_SETBIT(a, i)	(a[i / STRESS_NBITS(a)] |= \
				 (TYPEOF_CAST(a[0])1U << (i & (STRESS_NBITS(a)-1))))

#define SIZEOF_ARRAY(a)		(sizeof(a) / sizeof(a[0]))

/* stress process prototype */
typedef int (*stress_func_t)(const stress_args_t *args);

/* perf related constants */
#if defined(HAVE_LIB_PTHREAD) &&	\
    defined(HAVE_LINUX_PERF_EVENT_H) &&	\
    defined(__NR_perf_event_open)
#define STRESS_PERF_STATS	(1)
#define STRESS_PERF_INVALID	(~0ULL)
#define STRESS_PERF_MAX		(128 + 16)

/* per perf counter info */
typedef struct {
	uint64_t counter;		/* perf counter */
	int	 fd;			/* perf per counter fd */
	uint8_t	 padding[4];		/* padding */
} stress_perf_stat_t;

/* per stressor perf info */
typedef struct {
	stress_perf_stat_t perf_stat[STRESS_PERF_MAX]; /* perf counters */
	int perf_opened;		/* count of opened counters */
	uint8_t	padding[4];		/* padding */
} stress_perf_t;
#endif

/* linux thermal zones */
#define	STRESS_THERMAL_ZONES	 (1)
#define STRESS_THERMAL_ZONES_MAX (31)	/* best if prime */

#if defined(STRESS_THERMAL_ZONES)
/* per stressor thermal zone info */
typedef struct stress_tz_info {
	char	*path;			/* thermal zone path */
	char 	*type;			/* thermal zone type */
	uint32_t type_instance;		/* thermal zone instance # */
	size_t	index;			/* thermal zone # index */
	struct stress_tz_info *next;	/* next thermal zone in list */
} stress_tz_info_t;

typedef struct {
	uint64_t temperature;		/* temperature in Celsius * 1000 */
} stress_tz_stat_t;

typedef struct {
	stress_tz_stat_t tz_stat[STRESS_THERMAL_ZONES_MAX];
} stress_tz_t;
#endif

typedef struct {
	uint64_t count_start;
	uint64_t count_stop;
} stress_interrupts_t;

/* Per stressor statistics and accounting info */
typedef struct {
	stress_counter_info_t ci;	/* counter info */
	double start;			/* wall clock start time */
	double duration;		/* finish - start */
	uint64_t counter_total;		/* counter total */
	double duration_total;		/* wall clock duration */
	pid_t pid;			/* stressor pid */
	bool sigalarmed;		/* set true if signalled with SIGALRM */
	bool signalled;			/* set true if signalled with a kill */
	bool completed;			/* true if stressor completed */
#if defined(STRESS_PERF_STATS)
	stress_perf_t sp;		/* perf counters */
#endif
#if defined(STRESS_THERMAL_ZONES)
	stress_tz_t tz;			/* thermal zones */
#endif
	stress_checksum_t *checksum;	/* pointer to checksum data */
	stress_interrupts_t interrupts[STRESS_INTERRUPTS_MAX];
	stress_metrics_data_t metrics[STRESS_MISC_METRICS_MAX];
	double rusage_utime;		/* rusage user time */
	double rusage_stime;		/* rusage system time */
	double rusage_utime_total;	/* rusage user time */
	double rusage_stime_total;	/* rusage system time */
	long int rusage_maxrss;		/* rusage max RSS, 0 = unused */
} stress_stats_t;

typedef struct shared_heap {
	void *str_list_head;		/* list of heap strings */
	void *lock;			/* heap global lock */
	void *heap;			/* mmap'd heap */
	size_t heap_size;		/* heap size */
	size_t offset;			/* next free offset in current slab */
	bool out_of_memory;		/* true if allocation failed */
} shared_heap_t;

#define	STRESS_WARN_HASH_MAX		(128)

/* The stress-ng global shared memory segment */
typedef struct {
	size_t length;			/* Size of shared segment */
	double time_started;		/* Time when stressing started */
	const uint64_t zero;		/* zero'd 64 bit data */
	void *nullptr;			/* Null pointer */
	uint64_t klog_errors;		/* Number of errors detected in klog */
	bool caught_sigint;		/* True if SIGINT caught */
	pid_t (*vfork)(void);		/* vfork syscall */
	stress_mapped_t mapped;		/* mmap'd pages to help testing */
	shared_heap_t shared_heap;
	struct {
		void *lock;		/* Cacheline stressor lock */
		int index;		/* Cacheline stressor index */
		uint8_t *buffer;	/* Cacheline stressor buffer */
		size_t size;		/* Cacheline buffer size */
	} cacheline;
	struct {
		uint32_t started;	/* Number of stressors started */
		uint32_t exited;	/* Number of stressors exited */
		uint32_t reaped;	/* Number of stressors reaped */
		uint32_t failed;	/* Number of stressors failed */
		uint32_t alarmed;	/* Number of stressors got SIGALRM */
	} instance_count;
	struct {
		uint8_t	*buffer;	/* Shared memory cache buffer */
		uint64_t size;		/* buffer size in bytes */
		uint16_t level;		/* 1=L1, 2=L2, 3=L3 */
		uint16_t padding1;	/* alignment padding */
		uint32_t ways;		/* cache ways size */
	} mem_cache;
#if defined(HAVE_ATOMIC_COMPARE_EXCHANGE) &&	\
    defined(HAVE_ATOMIC_STORE)
	struct {
		double whence;		/* pr_* lock time */
		pid_t atomic_lock;	/* pr_* atomic spinlock */
		int lock_count;		/* pr_* lock count, release when zero */
		pid_t pid;		/* pid owning the lock */
	} pr;
#endif
	struct {
		uint32_t hash[STRESS_WARN_HASH_MAX]; /* hash patterns */
		void *lock;		/* protection lock */
	} warn_once;
	union {
		uint64_t val64[1] ALIGN64;
		uint32_t val32[2] ALIGN64;
		uint16_t val16[4] ALIGN64;
		uint8_t	 val8[8] ALIGN64;
	} atomic;			/* Shared atomic temp vars */
	struct {
		/* futexes must be aligned to avoid -EINVAL */
		uint32_t futex[STRESS_PROCS_MAX] ALIGNED(4);/* Shared futexes */
		uint64_t timeout[STRESS_PROCS_MAX];	/* Shared futex timeouts */
	} futex;
#if defined(HAVE_SEM_SYSV) && 	\
    defined(HAVE_KEY_T)
	struct {
		key_t key_id;		/* System V semaphore key id */
		int sem_id;		/* System V semaphore id */
		bool init;		/* System V semaphore initialized */
	} sem_sysv;
#endif
#if defined(STRESS_PERF_STATS)
	struct {
		bool no_perf;		/* true = Perf not available */
		void *lock;		/* lock on no_perf updates */
	} perf;
#endif
#if defined(STRESS_THERMAL_ZONES)
	stress_tz_info_t *tz_info;	/* List of valid thermal zones */
#endif
	struct {
		double start_time ALIGNED(8);	/* Time to complete operation */
		uint32_t value;		/* Dummy value to operate on */
	} syncload;
	struct {
		stress_checksum_t *checksums;	/* per stressor counter checksum */
		size_t	length;		/* size of checksums mapping */
	} checksum;
	struct {
		uint8_t allocated[65536 / sizeof(uint8_t)];	/* allocation bitmap */
		void *lock;		/* lock for allocator */
	} net_port_map;
	struct {
		uint32_t ready;		/* incremented when rawsock stressor is ready */
	} rawsock;
	stress_stats_t stats[];		/* Shared statistics */
} stress_shared_t;

/* stress test metadata */
typedef struct {
	const stressor_info_t *info;	/* stress test info */
	const unsigned int id;		/* stress test ID */
	const short int short_getopt;	/* getopt short option */
	const stress_op_t op;		/* ops option */
	const char *name;		/* name of stress test */
} stress_t;

#define STRESS_STRESSOR_STATUS_PASSED		(0)
#define STRESS_STRESSOR_STATUS_FAILED		(1)
#define STRESS_STRESSOR_STATUS_SKIPPED		(2)
#define STRESS_STRESSOR_STATUS_BAD_METRICS	(3)
#define STRESS_STRESSOR_STATUS_MAX		(4)

/* stress_stressor_info ignore value. 2 bits */
#define STRESS_STRESSOR_NOT_IGNORED		(0)
#define STRESS_STRESSOR_UNSUPPORTED		(1)
#define STRESS_STRESSOR_EXCLUDED		(2)

/* Per stressor information */
typedef struct stress_stressor_info {
	struct stress_stressor_info *next; /* next proc info struct in list */
	struct stress_stressor_info *prev; /* prev proc info struct in list */
	const stress_t *stressor;	/* stressor */
	stress_stats_t **stats;		/* stressor stats info */
	int32_t completed_instances;	/* count of completed instances */
	int32_t num_instances;		/* number of instances per stressor */
	uint64_t bogo_ops;		/* number of bogo ops */
	uint32_t status[STRESS_STRESSOR_STATUS_MAX];
					/* number of instances that passed/failed/skipped */
	struct {
		uint8_t run;		/* ignore running the stressor, unsupported or excluded */
		bool	permute;	/* ignore flag, saved for permute */
	} ignore;
} stress_stressor_t;


/* Pointer to current running stressor proc info */
extern stress_stressor_t *g_stressor_current;

/* Various global option settings and flags */
extern const char g_app_name[];		/* Name of application */
extern stress_shared_t *g_shared;	/* shared memory */
extern uint64_t	g_opt_timeout;		/* timeout in seconds */
extern uint64_t	g_opt_flags;		/* option flags */
extern volatile bool g_stress_continue_flag; /* false to exit stressor */
extern jmp_buf g_error_env;		/* parsing error env */

/*
 *  stress_continue_flag()
 *	get stress_continue_flag state
 */
static inline bool ALWAYS_INLINE OPTIMIZE3 stress_continue_flag(void)
{
	return g_stress_continue_flag;
}

/*
 *  stress_continue_set_flag()
 *	set stress_continue_flag state
 */
static inline void ALWAYS_INLINE OPTIMIZE3 stress_continue_set_flag(const bool setting)
{
	g_stress_continue_flag = setting;
}

/*
 *  stress_bogo_add()
 *	add inc to the stessor bogo ops counter
 *	NOTE: try to only add to the counter inside a stressor
 *	and not a child process of a stressor. If one has to add to
 *	the counter in a child and the child is force KILL'd then indicate
 *	so with the stress_force_killed_bogo() call from the parent.
 */
static inline void ALWAYS_INLINE OPTIMIZE3 stress_bogo_add(const stress_args_t *args, const uint64_t inc)
{
	register stress_counter_info_t * const ci = args->ci;

	ci->counter_ready = false;
	stress_asm_mb();
	ci->counter += inc;
	stress_asm_mb();
	ci->counter_ready = true;
}

/*
 *  stress_bogo_inc()
 *	increment the stessor bogo ops counter
 *	NOTE: try to only increment the counter inside a stressor
 *	and not a child process of a stressor. If one has to increment
 *	the counter in a child and the child is force KILL'd then indicate
 *	so with the stress_force_killed_bogo() call from the parent.
 */
static inline void ALWAYS_INLINE OPTIMIZE3 stress_bogo_inc(const stress_args_t *args)
{
	register stress_counter_info_t * const ci = args->ci;

	ci->counter_ready = false;
	stress_asm_mb();
	ci->counter++;
	stress_asm_mb();
	ci->counter_ready = true;
}

/*
 *  stress_bogo_get()
 *	get the stessor bogo ops counter
 */
static inline uint64_t ALWAYS_INLINE OPTIMIZE3 stress_bogo_get(const stress_args_t *args)
{
	register const stress_counter_info_t * const ci = args->ci;

	return ci->counter;
}

/*
 *  stress_bogo_set()
 *	set the stessor bogo ops counter
 *	NOTE: try to only set the counter inside a stressor
 *	and not a child process of a stressor. If one has to set
 *	the counter in a child and the child is force KILL'd then indicate
 *	so with the stress_force_killed_bogo() call from the parent.
 */
static inline void ALWAYS_INLINE OPTIMIZE3 stress_bogo_set(const stress_args_t *args, const uint64_t val)
{
	register stress_counter_info_t * const ci = args->ci;

	ci->counter_ready = false;
	stress_asm_mb();
	ci->counter = val;
	stress_asm_mb();
	ci->counter_ready = true;
}

/*
 *  stress_force_killed_bogo()
 *	note that the process is force killed and counter ready state can
 *	be ignored. Use only if the parent kills the child *and* the child
 *	was used to increment the bogo-op counter.
 */
static inline void ALWAYS_INLINE stress_force_killed_bogo(const stress_args_t *args)
{
	args->ci->force_killed = true;
}

/*
 *  stress_continue()
 *      returns true if we can keep on running a stressor
 */
static inline bool ALWAYS_INLINE OPTIMIZE3 stress_continue(const stress_args_t *args)
{
	if (UNLIKELY(!g_stress_continue_flag))
		return false;
	if (LIKELY(args->max_ops == 0))
		return true;
	return stress_bogo_get(args) < args->max_ops;
}

/*
 *  stress_bogo_add_lock()
 *	add val to the stessor bogo ops counter with lock, return true
 *	if stress_continue is true
 */
static inline void stress_bogo_add_lock(const stress_args_t *args, void *lock, const int64_t val)
{
	/*
	 *  Failure in lock acquire, don't bump counter
	 *  and get racy stress_continue state, that's
	 *  probably the best we can do in this failure mode
	 */
	if (UNLIKELY(stress_lock_acquire(lock) < 0))
		return;
	stress_bogo_add(args, val);
	stress_lock_release(lock);
}

/*
 *  stress_bogo_inc_lock()
 *	increment the stessor bogo ops counter with lock, return true
 *	if stress_continue is true
 */
static inline bool stress_bogo_inc_lock(const stress_args_t *args, void *lock, const bool inc)
{
	bool ret;

	/*
	 *  Failure in lock acquire, don't bump counter
	 *  and get racy stress_continue state, that's
	 *  probably the best we can do in this failure mode
	 */
	if (UNLIKELY(stress_lock_acquire(lock) < 0))
		return stress_continue(args);
	ret = stress_continue(args);
	if (inc && ret)
		stress_bogo_inc(args);
	stress_lock_release(lock);

	return ret;
}

/*
 *  externs to force gcc to stash computed values and hence
 *  to stop the optimiser optimising code away to zero. The
 *  *_put funcs are essentially no-op functions.
 */
extern WARN_UNUSED uint64_t stress_get_uint64_zero(void);

/* Filenames and directories */
extern int stress_temp_filename(char *path, const size_t len,
	const char *name, const pid_t pid, const uint32_t instance,
	const uint64_t magic);
extern int stress_temp_filename_args(const stress_args_t *args, char *path,
	const size_t len, const uint64_t magic);
extern int stress_temp_dir(char *path, const size_t len,
	const char *name, const pid_t pid, const uint32_t instance);
extern int stress_temp_dir_args(const stress_args_t *args, char *path,
	const size_t len);
extern WARN_UNUSED int stress_temp_dir_mk(const char *name, const pid_t pid,
	const uint32_t instance);
extern WARN_UNUSED int stress_temp_dir_mk_args(const stress_args_t *args);
extern int stress_temp_dir_rm(const char *name, const pid_t pid,
	const uint32_t instance);
extern int stress_temp_dir_rm_args(const stress_args_t *args);
extern void stress_cwd_readwriteable(void);

extern const char *stress_get_signal_name(const int signum);
extern const char *stress_strsignal(const int signum);
extern WARN_UNUSED int stress_sigchld_set_handler(const stress_args_t *args);

/* Time handling */
extern WARN_UNUSED double stress_timeval_to_double(const struct timeval *tv);
extern WARN_UNUSED double stress_timespec_to_double(const struct timespec *ts);
extern WARN_UNUSED double stress_time_now(void);
extern const char *stress_duration_to_str(const double duration, const bool int_secs);

/* 64 and 32 char ASCII patterns */
extern const char ALIGN64 stress_ascii64[64];
extern const char ALIGN64 stress_ascii32[32];

/* Misc helpers */
extern size_t stress_mk_filename(char *fullname, const size_t fullname_len,
	const char *pathname, const char *filename);
extern void stress_set_proc_name_init(int argc, char *argv[], char *envp[]);
extern void stress_set_proc_name(const char *name);
extern void stress_set_proc_state_str(const char *name, const char *str);
extern void stress_set_proc_state(const char *name, const int state);
extern WARN_UNUSED int stress_get_unused_uid(uid_t *uid);
extern void NORETURN MLOCKED_TEXT stress_sig_handler_exit(int signum);
extern void stress_clear_warn_once(void);
extern WARN_UNUSED size_t stress_flag_permutation(const int flags, int **permutations);
extern WARN_UNUSED const char *stress_fs_magic_to_name(const unsigned long fs_magic);
extern WARN_UNUSED const char *stress_get_fs_type(const char *filename);
extern void stress_close_fds(int *fds, const size_t n);
extern void stress_file_rw_hint_short(const int fd);
extern void stress_set_vma_anon_name(const void *addr, const size_t size, const char *name);
extern void stress_clean_dir(const char *name, const pid_t pid, const uint32_t instance);

/* Misc helper funcs */
extern WARN_UNUSED size_t stress_get_sig_stack_size(void);
extern WARN_UNUSED size_t stress_get_min_sig_stack_size(void);
extern WARN_UNUSED size_t stress_get_min_pthread_stack_size(void);

#define STRESS_SIGSTKSZ		(stress_get_sig_stack_size())
#define STRESS_MINSIGSTKSZ	(stress_get_min_sig_stack_size())

extern void stress_shared_unmap(void);
extern void stress_log_system_mem_info(void);
extern size_t stress_munge_underscore(char *dst, const char *src, size_t len);
extern WARN_UNUSED int stress_strcmp_munged(const char *s1, const char *s2);
extern size_t stress_get_page_size(void);
extern WARN_UNUSED int32_t stress_get_processors_online(void);
extern WARN_UNUSED int32_t stress_get_processors_configured(void);
extern WARN_UNUSED int32_t stress_get_ticks_per_second(void);
extern WARN_UNUSED ssize_t stress_get_stack_direction(void);
extern WARN_UNUSED void *stress_get_stack_top(void *start, size_t size);
extern void stress_get_memlimits(size_t *shmall, size_t *freemem,
	size_t *totalmem, size_t *freeswap, size_t *totalswap);
extern WARN_UNUSED int stress_get_load_avg(double *min1, double *min5,
	double *min15);
extern void stress_parent_died_alarm(void);
extern int stress_process_dumpable(const bool dumpable);
extern int stress_set_timer_slack_ns(const char *opt);
extern void stress_set_timer_slack(void);
extern WARN_UNUSED int stress_set_temp_path(const char *path);
extern WARN_UNUSED const char *stress_get_temp_path(void);
extern WARN_UNUSED int stress_check_temp_path(void);
extern void stress_temp_path_free(void);
extern void stress_rndstr(char *str, size_t len);
extern void stress_rndbuf(void *str, const size_t len);
extern void stress_uint8rnd4(uint8_t *data, const size_t len);
extern WARN_UNUSED unsigned int stress_get_cpu(void);
extern WARN_UNUSED const char *stress_get_compiler(void);
extern WARN_UNUSED const char *stress_get_uname_info(void);
extern WARN_UNUSED int stress_cache_alloc(const char *name);
extern void stress_cache_free(void);
extern WARN_UNUSED int stress_set_nonblock(const int fd);
extern WARN_UNUSED ssize_t stress_system_read(const char *path, char *buf,
	const size_t buf_len);
extern ssize_t stress_system_write(const char *path, const char *buf,
	const size_t buf_len);
extern WARN_UNUSED bool stress_is_prime64(const uint64_t n);
extern WARN_UNUSED uint64_t stress_get_next_prime64(const uint64_t n);
extern WARN_UNUSED uint64_t stress_get_prime64(const uint64_t n);
extern WARN_UNUSED size_t stress_get_file_limit(void);
extern WARN_UNUSED size_t stress_get_max_file_limit(void);
extern WARN_UNUSED int stress_get_bad_fd(void);
extern WARN_UNUSED int stress_sigaltstack_no_check(void *stack, const size_t size);
extern WARN_UNUSED int stress_sigaltstack(void *stack, const size_t size);
extern void stress_sigaltstack_disable(void);
extern WARN_UNUSED int stress_sighandler(const char *name, const int signum,
	void (*handler)(int), struct sigaction *orig_action);
extern int stress_sighandler_default(const int signum);
extern void stress_handle_stop_stressing(const int signum);
extern WARN_UNUSED int stress_sig_stop_stressing(const char *name,
	const int sig);
extern int stress_sigrestore(const char *name, const int signum,
	struct sigaction *orig_action);
extern WARN_UNUSED int stress_unimplemented(const stress_args_t *args);
extern WARN_UNUSED size_t stress_probe_max_pipe_size(void);
extern WARN_UNUSED void *stress_align_address(const void *addr,
	const size_t alignment);
extern WARN_UNUSED uint64_t stress_get_phys_mem_size(void);
extern WARN_UNUSED uint64_t stress_get_filesystem_size(void);
extern WARN_UNUSED ssize_t stress_read_buffer(const int fd, void* buffer,
	const ssize_t size, const bool ignore_sig_eintr);
extern WARN_UNUSED ssize_t stress_write_buffer(const int fd, const void* buffer,
	const ssize_t size, const bool ignore_sig_eintr);
extern WARN_UNUSED uint64_t stress_get_filesystem_available_inodes(void);
extern WARN_UNUSED int stress_kernel_release(const int major,
	const int minor, const int patchlevel);
extern WARN_UNUSED int stress_get_kernel_release(void);
extern char *stress_uint64_to_str(char *str, size_t len, const uint64_t val);
extern WARN_UNUSED int stress_drop_capabilities(const char *name);
extern WARN_UNUSED bool stress_is_dot_filename(const char *name);
extern WARN_UNUSED char *stress_const_optdup(const char *opt);
extern size_t stress_exec_text_addr(char **start, char **end);
extern WARN_UNUSED bool stress_check_capability(const int capability);
extern WARN_UNUSED bool stress_sigalrm_pending(void);
extern WARN_UNUSED bool stress_is_dev_tty(const int fd);
extern WARN_UNUSED bool stress_little_endian(void);
extern WARN_UNUSED char *stress_get_proc_self_exe(char *path, const size_t path_len);
extern WARN_UNUSED int stress_bsd_getsysctl(const char *name, void *ptr, size_t size);
extern WARN_UNUSED uint64_t stress_bsd_getsysctl_uint64(const char *name);
extern WARN_UNUSED uint32_t stress_bsd_getsysctl_uint32(const char *name);
extern WARN_UNUSED unsigned int stress_bsd_getsysctl_uint(const char *name);
extern WARN_UNUSED int stress_bsd_getsysctl_int(const char *name);

extern void stress_dirent_list_free(struct dirent **dlist, const int n);
extern WARN_UNUSED int stress_dirent_list_prune(struct dirent **dlist, const int n);
extern WARN_UNUSED uint16_t stress_ipv4_checksum(uint16_t *ptr, const size_t n);
extern int stress_read_fdinfo(const pid_t pid, const int fd);
extern WARN_UNUSED pid_t stress_get_unused_pid_racy(const bool fork_test);
extern WARN_UNUSED size_t stress_get_hostname_length(void);

extern void stress_metrics_set_const_check(const stress_args_t *args,
	const size_t idx, char *description, const bool const_description, const double value);
#if defined(HAVE_BUILTIN_CONSTANT_P)
#define stress_metrics_set(args, idx, description, value)	\
	stress_metrics_set_const_check(args, idx, description, __builtin_constant_p(description), value)
#else
#define stress_metrics_set(args, idx, description, value)	\
	stress_metrics_set_const_check(args, idx, description, false, value)
#endif
extern WARN_UNUSED int stress_get_tty_width(void);
extern WARN_UNUSED size_t stress_get_extents(const int fd);
extern WARN_UNUSED bool stress_redo_fork(const stress_args_t *args, const int err);
extern void stress_sighandler_nop(int sig);
extern int stress_killpid(const pid_t pid);
extern WARN_UNUSED bool stress_low_memory(const size_t requested);
extern void stress_ksm_memory_merge(const int flag);
extern WARN_UNUSED int stress_x86_smi_readmsr64(const int cpu, const uint32_t reg, uint64_t *val);
extern void stress_unset_chattr_flags(const char *pathname);
extern int stress_munmap_retry_enomem(void *addr, size_t length);
extern int stress_swapoff(const char *path);

extern WARN_UNUSED int stress_exit_status(const int err);

/*
 *  Stack aligning for clone() system calls
 *	align to nearest 16 bytes for aarch64 et al,
 *	assumes we have enough slop to do this
 */
static inline WARN_UNUSED ALWAYS_INLINE void *stress_align_stack(void *stack_top)
{
	return (void *)((uintptr_t)stack_top & ~(uintptr_t)0xf);
}

/*
 *  stress_warn_once hashes the current filename and line where
 *  the macro is used and returns true if it's never been called
 *  there before across all threads and child processes
 */
extern WARN_UNUSED bool stress_warn_once_hash(const char *filename, const int line);
#define stress_warn_once()	stress_warn_once_hash(__FILE__, __LINE__)

extern WARN_UNUSED int stress_parse_opts(int argc, char **argv, const bool jobmode);

/* Memory tweaking */
extern int stress_pagein_self(const char *name);

/* Used to set options for specific stressors */
extern void stress_adjust_pthread_max(const uint64_t max);
extern void stress_adjust_sleep_max(const uint64_t max);

/* Enable/disable stack smashing error message */
extern void stress_set_stack_smash_check_flag(const bool flag);

#if !defined(STRESS_CORE_SHIM) &&	\
    !defined(HAVE_PEDANTIC) &&		\
    (defined(HAVE_COMPILER_GCC) && defined(HAVE_COMPILER_CLANG))
int unlink(const char *pathname) __attribute__((deprecated("use shim_unlink")));
int unlinkat(int dirfd, const char *pathname, int flags) __attribute__((deprecated("use shim_unlinkat")));
int rmdir(const char *pathname) __attribute__((deprecated("use shim_rmdir")));
#endif

#endif
