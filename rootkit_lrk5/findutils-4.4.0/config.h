/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define so that source code can verify that config.h was already included */
#define ALREADY_INCLUDED_CONFIG_H 1

/* Define to the number of bits in type 'ptrdiff_t'. */
/* #undef BITSIZEOF_PTRDIFF_T */

/* Define to the number of bits in type 'sig_atomic_t'. */
/* #undef BITSIZEOF_SIG_ATOMIC_T */

/* Define to the number of bits in type 'size_t'. */
/* #undef BITSIZEOF_SIZE_T */

/* Define to the number of bits in type 'wchar_t'. */
/* #undef BITSIZEOF_WCHAR_T */

/* Define to the number of bits in type 'wint_t'. */
/* #undef BITSIZEOF_WINT_T */

/* Define if you want find -nouser and -nogroup to make tables of used UIDs
   and GIDs at startup instead of using getpwuid or getgrgid when needed.
   Speeds up -nouser and -nogroup unless you are running NIS or Hesiod, which
   make password and group calls very expensive. */
/* #undef CACHE_IDS */

/* Define if chown is not POSIX compliant regarding IDs of -1. */
/* #undef CHOWN_FAILS_TO_HONOR_ID_OF_NEGATIVE_ONE */

/* Define if chown modifies symlinks. */
/* #undef CHOWN_MODIFIES_SYMLINK */

/* Define to 1 if the `closedir' function returns void instead of `int'. */
/* #undef CLOSEDIR_VOID */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Define if you want to see find's innards */
/* #undef DEBUG */

/* Define if you want to see a message every time find calls the stat() system
   call */
/* #undef DEBUG_STAT */

/* If defined, the default argument size used in child processes */
/* #undef DEFAULT_ARG_SIZE */

/* the name of the file descriptor member of DIR */
/* #undef DIR_FD_MEMBER_NAME */

#ifdef DIR_FD_MEMBER_NAME
# define DIR_TO_FD(Dir_p) ((Dir_p)->DIR_FD_MEMBER_NAME)
#else
# define DIR_TO_FD(Dir_p) -1
#endif


/* Define to 1 if // is a file system root distinct from /. */
/* #undef DOUBLE_SLASH_IS_DISTINCT_ROOT */

/* Define if struct dirent has a member d_ino that actually works. */
#define D_INO_IN_DIRENT 1

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#define ENABLE_NLS 1

/* Define if gnulib's fchdir() replacement is used. */
/* #undef FCHDIR_REPLACEMENT */

/* Define on systems for which file names may have a so-called `drive letter'
   prefix, define this to compute the length of that prefix, including the
   colon. */
#define FILE_SYSTEM_ACCEPTS_DRIVE_LETTER_PREFIX 0

/* Define if the backslash character may also serve as a file name component
   separator. */
#define FILE_SYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR 0

/* Define if a drive letter prefix denotes a relative path if it is not
   followed by a file name component separator. */
#define FILE_SYSTEM_DRIVE_PREFIX_CAN_BE_RELATIVE 0

/* Define if we are compiling GNU findutils */
#define FINDUTILS 1

/* Define to nothing if C supports flexible array members, and to 1 if it does
   not. That way, with a declaration like `struct s { int n; double
   d[FLEXIBLE_ARRAY_MEMBER]; };', the struct hack can be used with pre-C99
   compilers. When computing the size of such an object, don't use 'sizeof
   (struct s)' as it overestimates the size. Use 'offsetof (struct s, d)'
   instead. Don't use 'offsetof (struct s, d[0])', as this doesn't work with
   MSVC and with C++ compilers. */
#define FLEXIBLE_ARRAY_MEMBER 

/* Define if gettimeofday clobbers the localtime buffer. */
/* #undef GETTIMEOFDAY_CLOBBERS_LOCALTIME */

/* Define to make the limit macros in <stdint.h> visible. */
#define GL_TRIGGER_STDC_LIMIT_MACROS 1

/* Define to 1 when using the gnulib module canonicalize. */
#define GNULIB_CANONICALIZE 1

/* Define to 1 when using the gnulib module close-stream. */
#define GNULIB_CLOSE_STREAM 1

/* Define to 1 when using the gnulib module fcntl-safer. */
#define GNULIB_FCNTL_SAFER 1

/* Define to 1 when using the gnulib module fopen-safer. */
#define GNULIB_FOPEN_SAFER 1

/* Define to 1 when using the gnulib module fts. */
#define GNULIB_FTS 1

/* Define to indicate the 'malloc' module. */
#define GNULIB_MALLOC_GNU 1

/* Define to indicate the 'realloc' module. */
#define GNULIB_REALLOC_GNU 1

/* Define to 1 if you have the `alarm' function. */
#define HAVE_ALARM 1

/* Define to 1 if you have 'alloca' after including <alloca.h>, a header that
   may be supplied by this distribution. */
#define HAVE_ALLOCA 1

/* Define HAVE_ALLOCA_H for backward compatibility with older code that
   includes <alloca.h> only if HAVE_ALLOCA_H is defined. */
#define HAVE_ALLOCA_H 1

/* Define to 1 if the compiler supports __attribute__ ((__noreturn__)) */
#define HAVE_ATTRIBUTE_NORETURN 1

/* Define to 1 if you have the <bp-sym.h> header file. */
/* #undef HAVE_BP_SYM_H */

/* Define to 1 if you have the `btowc' function. */
#define HAVE_BTOWC 1

/* Define to 1 if you have the `canonicalize_file_name' function. */
#define HAVE_CANONICALIZE_FILE_NAME 1

/* Define to 1 if you have the MacOS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the MacOS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Define to 1 if your system has a working `chown' function. */
#define HAVE_CHOWN 1

/* Define to 1 if you have the `clock_gettime' function. */
#define HAVE_CLOCK_GETTIME 1

/* Define to 1 if you have the `clock_settime' function. */
#define HAVE_CLOCK_SETTIME 1

/* Define if you have compound literals. */
/* #undef HAVE_COMPOUND_LITERALS */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
#define HAVE_DCGETTEXT 1

/* Define to 1 if you have the declaration of `canonicalize_file_name', and to
   0 if you don't. */
#define HAVE_DECL_CANONICALIZE_FILE_NAME 1

/* Define to 1 if you have the declaration of `dirfd', and to 0 if you don't.
   */
#define HAVE_DECL_DIRFD 1

/* Define to 1 if you have the declaration of `fpurge', and to 0 if you don't.
   */
#define HAVE_DECL_FPURGE 0

/* Define to 1 if you have the declaration of `getcwd', and to 0 if you don't.
   */
#define HAVE_DECL_GETCWD 1

/* Define to 1 if you have the declaration of `getc_unlocked', and to 0 if you
   don't. */
#define HAVE_DECL_GETC_UNLOCKED 1

/* Define to 1 if you have the declaration of `getdelim', and to 0 if you
   don't. */
#define HAVE_DECL_GETDELIM 1

/* Define to 1 if you have the declaration of `getenv', and to 0 if you don't.
   */
#define HAVE_DECL_GETENV 1

/* Define to 1 if you have the declaration of `getline', and to 0 if you
   don't. */
#define HAVE_DECL_GETLINE 1

/* Define to 1 if you have the declaration of `imaxabs', and to 0 if you
   don't. */
#define HAVE_DECL_IMAXABS 1

/* Define to 1 if you have the declaration of `imaxdiv', and to 0 if you
   don't. */
#define HAVE_DECL_IMAXDIV 1

/* Define to 1 if you have the declaration of `isblank', and to 0 if you
   don't. */
/* #undef HAVE_DECL_ISBLANK */

/* Define to 1 if you have the declaration of `memrchr', and to 0 if you
   don't. */
#define HAVE_DECL_MEMRCHR 1

/* Define to 1 if you have the declaration of `mkdir', and to 0 if you don't.
   */
#define HAVE_DECL_MKDIR 1

/* Define to 1 if you have the declaration of `strdup', and to 0 if you don't.
   */
#define HAVE_DECL_STRDUP 1

/* Define to 1 if you have the declaration of `strerror', and to 0 if you
   don't. */
/* #undef HAVE_DECL_STRERROR */

/* Define to 1 if you have the declaration of `strerror_r', and to 0 if you
   don't. */
#define HAVE_DECL_STRERROR_R 1

/* Define to 1 if you have the declaration of `strmode', and to 0 if you
   don't. */
#define HAVE_DECL_STRMODE 0

/* Define to 1 if you have the declaration of `strndup', and to 0 if you
   don't. */
#define HAVE_DECL_STRNDUP 1

/* Define to 1 if you have the declaration of `strnlen', and to 0 if you
   don't. */
#define HAVE_DECL_STRNLEN 1

/* Define to 1 if you have the declaration of `strtoimax', and to 0 if you
   don't. */
#define HAVE_DECL_STRTOIMAX 1

/* Define to 1 if you have the declaration of `strtoll', and to 0 if you
   don't. */
/* #undef HAVE_DECL_STRTOLL */

/* Define to 1 if you have the declaration of `strtoull', and to 0 if you
   don't. */
/* #undef HAVE_DECL_STRTOULL */

/* Define to 1 if you have the declaration of `strtoumax', and to 0 if you
   don't. */
#define HAVE_DECL_STRTOUMAX 1

/* Define to 1 if you have the declaration of `tzname', and to 0 if you don't.
   */
/* #undef HAVE_DECL_TZNAME */

/* Define to 1 if you have the declaration of `wcwidth', and to 0 if you
   don't. */
#define HAVE_DECL_WCWIDTH 1

/* Define to 1 if you have the declaration of `__fpending', and to 0 if you
   don't. */
#define HAVE_DECL___FPENDING 1

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the `dirfd' function. */
#define HAVE_DIRFD 1

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define to 1 if you have the `dup2' function. */
#define HAVE_DUP2 1

/* Define to 1 if you have the `endgrent' function. */
#define HAVE_ENDGRENT 1

/* Define to 1 if you have the `endmntent' function. */
#define HAVE_ENDMNTENT 1

/* Define to 1 if you have the `endpwent' function. */
#define HAVE_ENDPWENT 1

/* Define if you have the declaration of environ. */
/* #undef HAVE_ENVIRON_DECL */

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* fabs is defined in -lm */
#define HAVE_FABS_IN_LIBM 1

/* Define to 1 if you have the `fchdir' function. */
#define HAVE_FCHDIR 1

/* Define to 1 if you have the `fchmodat' function. */
#define HAVE_FCHMODAT 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fdopendir' function. */
#define HAVE_FDOPENDIR 1

/* Define to 1 if you have the `flockfile' function. */
/* #undef HAVE_FLOCKFILE */

/* Define to 1 if you have the `forcefindlib' function. */
/* #undef HAVE_FORCEFINDLIB */

/* Define to 1 if you have the `fpurge' function. */
/* #undef HAVE_FPURGE */

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Define to 1 if you have the <fs_info.h> header file. */
/* #undef HAVE_FS_INFO_H */

/* Define to 1 if you have the `fs_stat_dev' function. */
/* #undef HAVE_FS_STAT_DEV */

/* Define to 1 if you have the `funlockfile' function. */
/* #undef HAVE_FUNLOCKFILE */

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the `getdelim' function. */
#define HAVE_GETDELIM 1

/* Define to 1 if you have the `getmntent' function. */
#define HAVE_GETMNTENT 1

/* Define to 1 if you have the `getmntinfo' function. */
/* #undef HAVE_GETMNTINFO */

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long_only' function. */
#define HAVE_GETOPT_LONG_ONLY 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define if the GNU gettext() function is already present or preinstalled. */
#define HAVE_GETTEXT 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the `hasmntopt' function. */
/* #undef HAVE_HASMNTOPT */

/* Define if you have the iconv() function and it works. */
/* #undef HAVE_ICONV */

/* Define if your compiler supports the #include_next directive. */
#define HAVE_INCLUDE_NEXT 1

/* Define to 1 if the compiler supports one of the keywords 'inline',
   '__inline__', '__inline' and effectively inlines functions marked as such.
   */
#define HAVE_INLINE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* Define to 1 if you have the `isblank' function. */
#define HAVE_ISBLANK 1

/* Define to 1 if you have the `iswcntrl' function. */
#define HAVE_ISWCNTRL 1

/* Define to 1 if you have the `iswctype' function. */
#define HAVE_ISWCTYPE 1

/* Define if you have <langinfo.h> and nl_langinfo(CODESET). */
#define HAVE_LANGINFO_CODESET 1

/* Define to 1 if you have the `lchmod' function. */
/* #undef HAVE_LCHMOD */

/* Define to 1 if you have the `lchown' function. */
#define HAVE_LCHOWN 1

/* Define to 1 if you have the `sun' library (-lsun). */
/* #undef HAVE_LIBSUN */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the `listmntent' function. */
/* #undef HAVE_LISTMNTENT */

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you support file names longer than 14 characters. */
#define HAVE_LONG_FILE_NAMES 1

/* Define to 1 if the system has the type `long long int'. */
#define HAVE_LONG_LONG_INT 1

/* Define to 1 if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#define HAVE_MALLOC 1

/* Define if the 'malloc' function is POSIX compliant. */
#define HAVE_MALLOC_POSIX 1

/* Define to 1 if you have the `mblen' function. */
#define HAVE_MBLEN 1

/* Define to 1 if you have the `mbrlen' function. */
#define HAVE_MBRLEN 1

/* Define to 1 if you have the `mbrtowc' function. */
#define HAVE_MBRTOWC 1

/* Define to 1 if you have the `mbsinit' function. */
#define HAVE_MBSINIT 1

/* Define to 1 if you have the `mbsrtowcs' function. */
#define HAVE_MBSRTOWCS 1

/* Define to 1 if <wchar.h> declares mbstate_t. */
#define HAVE_MBSTATE_T 1

/* Define to 1 if you have the `memchr' function. */
#define HAVE_MEMCHR 1

/* Define to 1 if you have the `memcmp' function. */
#define HAVE_MEMCMP 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mempcpy' function. */
#define HAVE_MEMPCPY 1

/* Define to 1 if you have the `memrchr' function. */
#define HAVE_MEMRCHR 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the `mkdirat' function. */
#define HAVE_MKDIRAT 1

/* Define to 1 if you have the <mntent.h> header file. */
#define HAVE_MNTENT_H 1

/* modf is defined in -lm */
#define HAVE_MODF_IN_LIBM 1

/* Define to 1 if you have the `nanotime' function. */
/* #undef HAVE_NANOTIME */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the `next_dev' function. */
/* #undef HAVE_NEXT_DEV */

/* Define to 1 if you have the `openat' function. */
#define HAVE_OPENAT 1

/* Define to 1 if getcwd works, except it sometimes fails when it shouldn't,
   setting errno to ERANGE, ENAMETOOLONG, or ENOENT. If __GETCWD_PREFIX is not
   defined, it doesn't matter whether HAVE_PARTLY_WORKING_GETCWD is defined.
   */
#define HAVE_PARTLY_WORKING_GETCWD 1

/* Define to 1 if you have the `pathconf' function. */
#define HAVE_PATHCONF 1

/* Define to 1 if you have the `pipe' function. */
#define HAVE_PIPE 1

/* Define to 1 if you have the `readlink' function. */
#define HAVE_READLINK 1

/* Define to 1 if your system has a GNU libc compatible `realloc' function,
   and to 0 otherwise. */
#define HAVE_REALLOC 1

/* Define if the 'realloc' function is POSIX compliant. */
#define HAVE_REALLOC_POSIX 1

/* Define to 1 if you have the `resolvepath' function. */
/* #undef HAVE_RESOLVEPATH */

/* Define to 1 if you have the `rpmatch' function. */
#define HAVE_RPMATCH 1

/* Define to 1 if you have the <search.h> header file. */
/* #undef HAVE_SEARCH_H */

/* Define to 1 if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define to 1 if you have the `setgroups' function. */
#define HAVE_SETGROUPS 1

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if you have the `setmntent' function. */
#define HAVE_SETMNTENT 1

/* Define to 1 if 'sig_atomic_t' is a signed integer type. */
/* #undef HAVE_SIGNED_SIG_ATOMIC_T */

/* Define to 1 if 'wchar_t' is a signed integer type. */
/* #undef HAVE_SIGNED_WCHAR_T */

/* Define to 1 if 'wint_t' is a signed integer type. */
/* #undef HAVE_SIGNED_WINT_T */

/* Define to 1 if stdbool.h conforms to C99. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio_ext.h> header file. */
#define HAVE_STDIO_EXT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `stpcpy' function. */
#define HAVE_STPCPY 1

/* Define to 1 if you have the `strcasestr' function. */
#define HAVE_STRCASESTR 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the `strerror_r' function. */
#define HAVE_STRERROR_R 1

/* Define to 1 if you have the `strftime' function. */
#define HAVE_STRFTIME 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the strndup() function and it works. */
#define HAVE_STRNDUP 1

/* Define to 1 if you have the `strspn' function. */
#define HAVE_STRSPN 1

/* Define to 1 if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the `strtoimax' function. */
#define HAVE_STRTOIMAX 1

/* Define to 1 if you have the `strtol' function. */
#define HAVE_STRTOL 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the `strtoull' function. */
#define HAVE_STRTOULL 1

/* Define to 1 if you have the `strtoumax' function. */
#define HAVE_STRTOUMAX 1

/* Define to 1 if your 'struct dirent' has a d_type member */
#define HAVE_STRUCT_DIRENT_D_TYPE 1

/* Define to 1 if `f_fstypename' is member of `struct fsstat'. */
/* #undef HAVE_STRUCT_FSSTAT_F_FSTYPENAME */

/* Define to 1 if `f_fstypename' is member of `struct statfs'. */
/* #undef HAVE_STRUCT_STATFS_F_FSTYPENAME */

/* Define to 1 if `st_atimensec' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_ATIMENSEC */

/* Define to 1 if `st_atimespec.tv_nsec' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_ATIMESPEC_TV_NSEC */

/* Define to 1 if `st_atim.st__tim.tv_nsec' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_ATIM_ST__TIM_TV_NSEC */

/* Define to 1 if `st_atim.tv_nsec' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_ATIM_TV_NSEC 1

/* Define to 1 if `st_birthtimensec' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_BIRTHTIMENSEC */

/* Define to 1 if `st_birthtimespec.tv_nsec' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_NSEC */

/* Define to 1 if `st_birthtim.tv_nsec' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_BIRTHTIM_TV_NSEC */

/* Define to 1 if `st_blocks' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_BLOCKS 1

/* Define to 1 if `st_rdev' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_RDEV 1

/* Define to 1 if `tm_zone' is member of `struct tm'. */
#define HAVE_STRUCT_TM_TM_ZONE 1

/* Define to 1 if your `struct stat' has `st_blocks'. Deprecated, use
   `HAVE_STRUCT_STAT_ST_BLOCKS' instead. */
#define HAVE_ST_BLOCKS 1

/* Define if struct stat has an st_dm_mode member. */
/* #undef HAVE_ST_DM_MODE */

/* Define to 1 if you have the <sys/bitypes.h> header file. */
/* #undef HAVE_SYS_BITYPES_H */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/fs_types.h> header file. */
/* #undef HAVE_SYS_FS_TYPES_H */

/* Define to 1 if you have the <sys/inttypes.h> header file. */
/* #undef HAVE_SYS_INTTYPES_H */

/* Define to 1 if you have the <sys/mkdev.h> header file. */
/* #undef HAVE_SYS_MKDEV_H */

/* Define to 1 if you have the <sys/mntent.h> header file. */
/* #undef HAVE_SYS_MNTENT_H */

/* Define to 1 if you have the <sys/mntio.h> header file. */
/* #undef HAVE_SYS_MNTIO_H */

/* Define to 1 if you have the <sys/mnttab.h> header file. */
/* #undef HAVE_SYS_MNTTAB_H */

/* Define to 1 if you have the <sys/mount.h> header file. */
#define HAVE_SYS_MOUNT_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/statvfs.h> header file. */
#define HAVE_SYS_STATVFS_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/timeb.h> header file. */
/* #undef HAVE_SYS_TIMEB_H */

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/ucred.h> header file. */
/* #undef HAVE_SYS_UCRED_H */

/* Define to 1 if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define if struct tm has the tm_gmtoff member. */
#define HAVE_TM_GMTOFF 1

/* Define to 1 if your `struct tm' has `tm_zone'. Deprecated, use
   `HAVE_STRUCT_TM_TM_ZONE' instead. */
#define HAVE_TM_ZONE 1

/* Define to 1 if you have the `tsearch' function. */
/* #undef HAVE_TSEARCH */

/* Define to 1 if you don't have `tm_zone' but do have the external array
   `tzname'. */
/* #undef HAVE_TZNAME */

/* Define to 1 if you have the `tzset' function. */
#define HAVE_TZSET 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `unsetenv' function. */
#define HAVE_UNSETENV 1

/* Define to 1 if the system has the type `unsigned long long int'. */
#define HAVE_UNSIGNED_LONG_LONG_INT 1

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if you have the `waitpid' function. */
#define HAVE_WAITPID 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define if you have the 'wchar_t' type. */
#define HAVE_WCHAR_T 1

/* Define to 1 if you have the `wcrtomb' function. */
#define HAVE_WCRTOMB 1

/* Define to 1 if you have the `wcscoll' function. */
#define HAVE_WCSCOLL 1

/* Define to 1 if you have the <wctype.h> header file. */
#define HAVE_WCTYPE_H 1

/* Define to 1 if you have the `wcwidth' function. */
#define HAVE_WCWIDTH 1

/* Define if you have the 'wint_t' type. */
#define HAVE_WINT_T 1

/* Define to 1 if you have the `wmemchr' function. */
#define HAVE_WMEMCHR 1

/* Define to 1 if you have the `wmemcpy' function. */
#define HAVE_WMEMCPY 1

/* Define to 1 if you have the `wmempcpy' function. */
#define HAVE_WMEMPCPY 1

/* Define to 1 if O_NOATIME works. */
#define HAVE_WORKING_O_NOATIME 1

/* Define to 1 if O_NOFOLLOW works. */
#define HAVE_WORKING_O_NOFOLLOW 1

/* Define to 1 if the system has the type `_Bool'. */
#define HAVE__BOOL 1

/* Define to 1 if you have the `_ftime' function. */
/* #undef HAVE__FTIME */

/* Define to 1 if you have the `__fpending' function. */
#define HAVE___FPENDING 1

/* Define to 1 if you have the `__fpurge' function. */
#define HAVE___FPURGE 1

/* Define to 1 if you have the `__freading' function. */
#define HAVE___FREADING 1

#if FILE_SYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR
# define ISSLASH(C) ((C) == '/' || (C) == '\\')
#else
# define ISSLASH(C) ((C) == '/')
#endif

/* Define if you want to use the leaf optimisation (this can still be turned
   off with -noleaf) */
#define LEAF_OPTIMISATION 1

/* Define to 1 if lseek does not detect pipes. */
/* #undef LSEEK_PIPE_BROKEN */

/* Define to 1 if `lstat' dereferences a symlink specified with a trailing
   slash. */
#define LSTAT_FOLLOWS_SLASHED_SYMLINK 1

/* Define to 1 if `major', `minor', and `makedev' are declared in <mkdev.h>.
   */
/* #undef MAJOR_IN_MKDEV */

/* Define to 1 if `major', `minor', and `makedev' are declared in
   <sysmacros.h>. */
/* #undef MAJOR_IN_SYSMACROS */

/* If malloc(0) is != NULL, define this to 1. Otherwise define this to 0. */
#define MALLOC_0_IS_NONNULL 1

/* Define if there is no specific function for reading the list of mounted
   file systems. fread will be used to read /etc/mnttab. (SVR2) */
/* #undef MOUNTED_FREAD */

/* Define if (like SVR2) there is no specific function for reading the list of
   mounted file systems, and your system has these header files: <sys/fstyp.h>
   and <sys/statfs.h>. (SVR3) */
/* #undef MOUNTED_FREAD_FSTYP */

/* Define if there are functions named next_dev and fs_stat_dev for reading
   the list of mounted file systems. (BeOS) */
/* #undef MOUNTED_FS_STAT_DEV */

/* Define if there is a function named getfsstat for reading the list of
   mounted file systems. (DEC Alpha running OSF/1) */
/* #undef MOUNTED_GETFSSTAT */

/* Define if there is a function named getmnt for reading the list of mounted
   file systems. (Ultrix) */
/* #undef MOUNTED_GETMNT */

/* Define if there is a function named getmntent for reading the list of
   mounted file systems, and that function takes a single argument. (4.3BSD,
   SunOS, HP-UX, Dynix, Irix) */
#define MOUNTED_GETMNTENT1 1

/* Define if there is a function named getmntent for reading the list of
   mounted file systems, and that function takes two arguments. (SVR4) */
/* #undef MOUNTED_GETMNTENT2 */

/* Define if there is a function named getmntinfo for reading the list of
   mounted file systems and it returns an array of 'struct statfs'. (4.4BSD,
   Darwin) */
/* #undef MOUNTED_GETMNTINFO */

/* Define if there is a function named getmntinfo for reading the list of
   mounted file systems and it returns an array of 'struct statvfs'. (NetBSD
   3.0) */
/* #undef MOUNTED_GETMNTINFO2 */

/* Define if there is a function named listmntent that can be used to list all
   mounted file systems. (UNICOS) */
/* #undef MOUNTED_LISTMNTENT */

/* Define if there is a function named mntctl that can be used to read the
   list of mounted file systems, and there is a system header file that
   declares `struct vmount.' (AIX) */
/* #undef MOUNTED_VMOUNT */

/* Define to 1 if assertions should be disabled. */
/* #undef NDEBUG */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "findutils"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "bug-findutils@gnu.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "GNU findutils"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "GNU findutils 4.4.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "findutils"

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.4.0"

/* the number of pending output bytes on stream `fp' */
/* #undef PENDING_OUTPUT_N_BYTES */

/* Define if <inttypes.h> exists and defines unusable PRI* macros. */
/* #undef PRI_MACROS_BROKEN */

/* Define to 1 if the C compiler supports function prototypes. */
#define PROTOTYPES 1

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'ptrdiff_t'. */
/* #undef PTRDIFF_T_SUFFIX */

/* Define this to 1 if strerror is broken. */
/* #undef REPLACE_STRERROR */

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'sig_atomic_t'. */
/* #undef SIG_ATOMIC_T_SUFFIX */

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'size_t'. */
/* #undef SIZE_T_SUFFIX */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at runtime.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */
/* #undef STAT_MACROS_BROKEN */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if strerror_r returns char *. */
#define STRERROR_R_CHAR_P 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Define to 1 if the type of the st_atim member of a struct stat is struct
   timespec. */
#define TYPEOF_STRUCT_STAT_ST_ATIM_IS_STRUCT_TIMESPEC 1

/* Defined if you specify --enable-d_type-optimisation to configure. */
#define USE_STRUCT_DIRENT_D_TYPE 1

/* Version number of package */
#define VERSION "4.4.0"

/* Define if unsetenv() returns void, not int. */
/* #undef VOID_UNSETENV */

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'wchar_t'. */
/* #undef WCHAR_T_SUFFIX */

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'wint_t'. */
/* #undef WINT_T_SUFFIX */

/* Define if you want to use fts() to do the filesystem search. */
#define WITH_FTS 1

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define if you want regoff_t to be at least as wide POSIX requires. */
/* #undef _REGEX_LARGE_OFFSETS */

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Define to rpl_ if the getopt replacement functions and variables should be
   used. */
/* #undef __GETOPT_PREFIX */

/* Define to rpl_ if the openat replacement function should be used. */
/* #undef __OPENAT_PREFIX */

/* Define like PROTOTYPES; this can be used by system headers. */
#define __PROTOTYPES 1

/* Ensure that <stdint.h> defines the limit macros, since gnulib's
   <inttypes.h> relies on them.  */
#if defined __cplusplus && !defined __STDC_LIMIT_MACROS && GL_TRIGGER_STDC_LIMIT_MACROS
# define __STDC_LIMIT_MACROS 1
#endif


/* Define to `unsigned long' if <sys/types.h> does not define. */
/* #undef dev_t */

/* Define to rpl_fchownat if the replacement function should be used. */
/* #undef fchownat */

/* Define to a replacement function name for fnmatch(). */
/* #undef fnmatch */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* A replacement for va_copy, if needed.  */
#define gl_va_copy(a,b) ((a) = (b))

/* Define to rpl_gmtime if the replacement function should be used. */
/* #undef gmtime */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `unsigned long' if <sys/types.h> does not define. */
/* #undef ino_t */

/* Define to rpl_localtime if the replacement function should be used. */
/* #undef localtime */

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to a type if <wchar.h> does not define. */
/* #undef mbstate_t */

/* Define to rpl_memcmp if the replacement function should be used. */
/* #undef memcmp */

/* Define to rpl_mktime if the replacement function should be used. */
/* #undef mktime */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to the name of the strftime replacement function. */
#define my_strftime nstrftime

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to rpl_re_comp if the replacement should be used. */
/* #undef re_comp */

/* Define to rpl_re_compile_fastmap if the replacement should be used. */
/* #undef re_compile_fastmap */

/* Define to rpl_re_compile_pattern if the replacement should be used. */
/* #undef re_compile_pattern */

/* Define to rpl_re_exec if the replacement should be used. */
/* #undef re_exec */

/* Define to rpl_re_match if the replacement should be used. */
/* #undef re_match */

/* Define to rpl_re_match_2 if the replacement should be used. */
/* #undef re_match_2 */

/* Define to rpl_re_search if the replacement should be used. */
/* #undef re_search */

/* Define to rpl_re_search_2 if the replacement should be used. */
/* #undef re_search_2 */

/* Define to rpl_re_set_registers if the replacement should be used. */
/* #undef re_set_registers */

/* Define to rpl_re_set_syntax if the replacement should be used. */
/* #undef re_set_syntax */

/* Define to rpl_re_syntax_options if the replacement should be used. */
/* #undef re_syntax_options */

/* Define to rpl_realloc if the replacement function should be used. */
/* #undef realloc */

/* Define to a replacement function name for realpath(). */
/* #undef realpath */

/* Define to rpl_regcomp if the replacement should be used. */
/* #undef regcomp */

/* Define to rpl_regerror if the replacement should be used. */
/* #undef regerror */

/* Define to rpl_regexec if the replacement should be used. */
/* #undef regexec */

/* Define to rpl_regfree if the replacement should be used. */
/* #undef regfree */

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#define restrict __restrict
/* Work around a bug in Sun C++: it does not support _Restrict, even
   though the corresponding Sun C compiler does, which causes
   "#define restrict _Restrict" in the previous line.  Perhaps some future
   version of Sun C++ will work with _Restrict; if so, it'll probably
   define __RESTRICT, just as Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */

/* Define to rpl_strnlen if the replacement function should be used. */
/* #undef strnlen */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

/* Define as a macro for copying va_list variables. */
/* #undef va_copy */
