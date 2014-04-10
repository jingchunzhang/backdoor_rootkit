
/* Option Flags */
typedef unsigned short opt_type;

#define OPT_VERBOSE 1
#define OPT_ALLFILES 2
#define OPT_MOUNTS 4
#define OPT_KILL 8
#define OPT_INTERACTIVE 16
#define OPT_SILENT 32
#define OPT_USER 64
#define OPT_ISMOUNTPOINT 128
#define OPT_WRITE 256

struct procs {
	pid_t pid;
	uid_t uid;
	char access;
	char proc_type;
	char *username;
	char *command;
	struct procs *next;
};

/* For the access field above */
#define ACCESS_CWD 1
#define ACCESS_EXE 2
#define ACCESS_FILE 4
#define ACCESS_ROOT 8
#define ACCESS_MMAP 16
#define ACCESS_FILEWR 32

/* For the proc_type field above */
#define PTYPE_NORMAL 0
#define PTYPE_MOUNT 1
#define PTYPE_KNFSD 2
#define PTYPE_SWAP 3

struct names {
	char *filename;
	unsigned char name_space;
    struct stat st;
	struct procs *matched_procs;
	struct names *next;
};

struct ip_connections {
	struct names *name;
	unsigned long lcl_port;
	unsigned long rmt_port;
	struct in_addr rmt_address;
	struct ip_connections *next;
};

struct ip6_connections {
	struct names *name;
	unsigned long lcl_port;
	unsigned long rmt_port;
	struct in6_addr rmt_address;
	struct ip6_connections *next;
};

struct inode_list {
	struct names *name;
	dev_t	device;
	ino_t	inode;
	struct inode_list *next;
};

struct device_list {
	struct names *name;
	dev_t	device;
	struct device_list *next;
};

struct unixsocket_list {
	char *sun_name;
	ino_t	inode;
	ino_t	net_inode;
	dev_t	dev;
	struct unixsocket_list *next;
};

struct mount_list {
	char *mountpoint;
	struct mount_list *next;
};

#if defined (__GNUC__) && defined(__OPTIMIZE__)
# include "lists.h"
typedef struct mntinfo_s {
    list_t   this;
    int id, parid;
    dev_t     dev;
    size_t   nlen;
    char  *mpoint;
} mntinfo_t;
#endif

#define NAMESPACE_FILE 0
#define NAMESPACE_TCP 1
#define NAMESPACE_UDP 2

#define MAX_PATHNAME 200
#define MAX_CMDNAME 16

#define KNFSD_EXPORTS "/proc/fs/nfs/exports"
#define PROC_MOUNTS "/proc/mounts"
#define PROC_SWAPS "/proc/swaps"
