/*
 * fuser.c - identify processes using files
 *
 * Based on fuser.c Copyright (C) 1993-2005 Werner Almesberger and Craig Small
 *
 * Completely re-written
 * Copyright (C) 2005-2011 Craig Small
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pwd.h>
#include <netdb.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <mntent.h>
#include <signal.h>
#include <getopt.h>
#include <setjmp.h>

#include "fuser.h"
#include "signals.h"
#include "i18n.h"

//#define DEBUG 1

#define NAME_FIELD 20		/* space reserved for file name */
/* Function defines */
static void add_matched_proc(struct names *name_list, const pid_t pid,
			     const uid_t uid, const char access);
static void add_special_proc(struct names *name_list, const char ptype,
			     const uid_t uid, const char *command);
static void check_dir(const pid_t pid, const char *dirname,
		      struct device_list *dev_head,
		      struct inode_list *ino_head, const uid_t uid,
		      const char access, struct unixsocket_list *sockets,
		      dev_t netdev);
static void check_map(const pid_t pid, const char *filename,
		      struct device_list *dev_head,
		      struct inode_list *ino_head, const uid_t uid,
		      const char access);
static struct stat *get_pidstat(const pid_t pid, const char *filename);
static uid_t getpiduid(const pid_t pid);
static int print_matches(struct names *names_head, const opt_type opts,
			 const int sig_number);
static int kill_matched_proc(struct procs *pptr, const opt_type opts,
			      const int sig_number);

/*int parse_mount(struct names *this_name, struct device_list **dev_list);*/
static void add_device(struct device_list **dev_list,
		       struct names *this_name, dev_t device);
void fill_unix_cache(struct unixsocket_list **unixsocket_head);
static dev_t find_net_dev(void);
static void scan_procs(struct names *names_head, struct inode_list *ino_head,
		       struct device_list *dev_head,
		       struct unixsocket_list *sockets, dev_t netdev);
static void scan_knfsd(struct names *names_head, struct inode_list *ino_head,
		       struct device_list *dev_head);
static void scan_mounts(struct names *names_head,
			struct inode_list *ino_head,
			struct device_list *dev_head);
static void scan_swaps(struct names *names_head, struct inode_list *ino_head,
		       struct device_list *dev_head);
#ifdef DEBUG
static void debug_match_lists(struct names *names_head,
			      struct inode_list *ino_head,
			      struct device_list *dev_head);
#endif

#ifdef _LISTS_H
static void clear_mntinfo(void) __attribute__((__destructor__));
static void init_mntinfo(void) __attribute__((__constructor__));
static dev_t device(const char *path);
#endif
static char *expandpath(const char *path);

typedef int (*stat_t)(const char*, struct stat*);
static int timeout(stat_t func, const char *path, struct stat *buf, unsigned int seconds);

static void usage(const char *errormsg)
{
	if (errormsg != NULL)
		fprintf(stderr, "%s\n", errormsg);

	fprintf(stderr,
		_
		("Usage: fuser [-fMuv] [-a|-s] [-4|-6] [-c|-m|-n SPACE] [-k [-i] [-SIGNAL]] NAME...\n"
		 "       fuser -l\n" "       fuser -V\n"
		 "Show which processes use the named files, sockets, or filesystems.\n\n"
		 "  -a,--all              display unused files too\n"
		 "  -i,--interactive      ask before killing (ignored without -k)\n"
		 "  -k,--kill             kill processes accessing the named file\n"
		 "  -l,--list-signals     list available signal names\n"
		 "  -m,--mount            show all processes using the named filesystems or block device\n"
		 "  -M,--ismountpoint     fulfill request only if NAME is a mount point\n"
		 "  -n,--namespace SPACE  search in this name space (file, udp, or tcp)\n"
		 "  -s,--silent           silent operation\n"
		 "  -SIGNAL               send this signal instead of SIGKILL\n"
		 "  -u,--user             display user IDs\n"
		 "  -v,--verbose          verbose output\n"
		 "  -w,--writeonly        kill only processes with write access\n"
		 "  -V,--version          display version information\n"));
#ifdef WITH_IPV6
	fprintf(stderr, _(
         "  -4,--ipv4             search IPv4 sockets only\n"
		 "  -6,--ipv6             search IPv6 sockets only\n"));
#endif
	fprintf(stderr, _(
         "  -                     reset options\n\n"
			  "  udp/tcp names: [local_port][,[rmt_host][,[rmt_port]]]\n\n"));
	exit(1);
}

void print_version()
{
	fprintf(stderr, _("fuser (PSmisc) %s\n"), VERSION);
	fprintf(stderr,
		_
		("Copyright (C) 1993-2010 Werner Almesberger and Craig Small\n\n"));
	fprintf(stderr,
		_("PSmisc comes with ABSOLUTELY NO WARRANTY.\n"
		  "This is free software, and you are welcome to redistribute it under\n"
		  "the terms of the GNU General Public License.\n"
		  "For more information about these matters, see the files named COPYING.\n"));
}

static void
scan_procs(struct names *names_head, struct inode_list *ino_head,
	   struct device_list *dev_head, struct unixsocket_list *sockets,
	   dev_t netdev)
{
	DIR *topproc_dir;
	struct dirent *topproc_dent;
	struct inode_list *ino_tmp;
	struct device_list *dev_tmp;
	pid_t pid, my_pid;
	uid_t uid;

	if ((topproc_dir = opendir("/proc")) == NULL) {
		fprintf(stderr, _("Cannot open /proc directory: %s\n"),
		strerror(errno));
		exit(1);
	}
	my_pid = getpid();
	while ((topproc_dent = readdir(topproc_dir)) != NULL) {
		dev_t cwd_dev, exe_dev, root_dev;
		struct stat *cwd_stat = NULL;
		struct stat *exe_stat = NULL;
		struct stat *root_stat = NULL;
#ifdef _LISTS_H
		char path[256] = "/proc/", *slash;
		ssize_t len;
#endif

		if (topproc_dent->d_name[0] < '0' || topproc_dent->d_name[0] > '9')	/* Not a process */
			continue;
		pid = atoi(topproc_dent->d_name);
		/* Dont print myself */
		if (pid == my_pid)
			continue;
		uid = getpiduid(pid);

#ifdef _LISTS_H
		strcpy(&path[6], topproc_dent->d_name);
		len = strlen(path);
		slash = &path[len];

		*slash = '\0';
		strcat(slash, "/cwd");
		cwd_dev = device(path);

		*slash = '\0';
		strcat(slash, "/exe");
		exe_dev = device(path);

		*slash = '\0';
		strcat(slash, "/root");
		root_dev = device(path);
#else
		cwd_stat = get_pidstat(pid, "cwd");
		exe_stat = get_pidstat(pid, "exe");
		root_stat = get_pidstat(pid, "root");
		cwd_dev = cwd_stat ? cwd_stat->st_dev : 0;
		exe_dev = exe_stat ? exe_stat->st_dev : 0;
		root_dev = root_stat ? root_stat->st_dev : 0;
#endif

		/* Scan the devices */
		for (dev_tmp = dev_head; dev_tmp != NULL; dev_tmp = dev_tmp->next) {
			if (exe_dev == dev_tmp->device)
				add_matched_proc(dev_tmp->name, pid, uid, ACCESS_EXE);
			if (root_dev == dev_tmp->device)
				add_matched_proc(dev_tmp->name, pid, uid, ACCESS_ROOT);
			if (cwd_dev == dev_tmp->device)
				add_matched_proc(dev_tmp->name, pid, uid, ACCESS_CWD);
		}
		for (ino_tmp = ino_head; ino_tmp != NULL; ino_tmp = ino_tmp->next) {
			if (exe_dev == ino_tmp->device) {
				if (!exe_stat)
					exe_stat = get_pidstat(pid, "exe");
				if (exe_stat && exe_stat->st_dev == ino_tmp->device
					     && exe_stat->st_ino == ino_tmp->inode)
					add_matched_proc(ino_tmp->name, pid, uid, ACCESS_EXE);
			}
			if (root_dev == ino_tmp->device) {
				if (!root_stat)
					root_stat = get_pidstat(pid, "root");
				if (root_stat && root_stat->st_dev == ino_tmp->device
					      && root_stat->st_ino == ino_tmp->inode)
					add_matched_proc(ino_tmp->name, pid, uid, ACCESS_ROOT);
			}
			if (cwd_dev == ino_tmp->device) {
				if (!cwd_stat)
					cwd_stat = get_pidstat(pid, "cwd");
				if (cwd_stat && cwd_stat->st_dev == ino_tmp->device
					     && cwd_stat->st_ino == ino_tmp->inode)
					add_matched_proc(ino_tmp->name, pid, uid, ACCESS_CWD);
			}
		}
		if (root_stat) free(root_stat);
		if (cwd_stat)  free(cwd_stat);
		if (exe_stat)  free(exe_stat);
#ifndef __linux__
		check_dir(pid, "lib", dev_head, ino_head, uid, ACCESS_MMAP,
			  sockets, netdev);
		check_dir(pid, "mmap", dev_head, ino_head, uid, ACCESS_MMAP,
			  sockets, netdev);
#endif
		check_dir(pid, "fd", dev_head, ino_head, uid, ACCESS_FILE,
			  sockets, netdev);
		check_map(pid, "maps", dev_head, ino_head, uid, ACCESS_MMAP);

	}			/* while topproc_dent */
	closedir(topproc_dir);
}

static void
add_inode(struct inode_list **ino_list, struct names *this_name,
	  dev_t device, ino_t inode)
{
	struct inode_list *ino_tmp, *ino_head;

	if ((ino_tmp = (struct inode_list*)malloc(sizeof(struct inode_list))) == NULL)
		return;
	ino_head = *ino_list;
	ino_tmp->name = this_name;
	ino_tmp->device = device;
	ino_tmp->inode = inode;
	ino_tmp->next = ino_head;
	*ino_list = ino_tmp;
}

static void
add_device(struct device_list **dev_list, struct names *this_name, dev_t device)
{
	struct device_list *dev_tmp, *dev_head;

	/*printf("Adding device %s %d\n", this_name->filename, device); */

	if ((dev_tmp = (struct device_list*)malloc(sizeof(struct device_list))) == NULL)
		return;
	dev_head = *dev_list;
	dev_tmp->name = this_name;
	dev_tmp->device = device;
	dev_tmp->next = dev_head;
	*dev_list = dev_tmp;
}

static void
add_ip_conn(struct ip_connections **ip_list, const char *protocol,
	    struct names *this_name, const int lcl_port, const int rmt_port,
	    unsigned long rmt_address)
{
	struct ip_connections *ip_tmp, *ip_head;

	if ((ip_tmp = (struct ip_connections*)malloc(sizeof(struct ip_connections))) == NULL)
		return;
	ip_head = *ip_list;
	ip_tmp->name = this_name;
	ip_tmp->lcl_port = lcl_port;
	ip_tmp->rmt_port = rmt_port;
	ip_tmp->rmt_address.s_addr = rmt_address;
	ip_tmp->next = ip_head;

	*ip_list = ip_tmp;
}

#ifdef WITH_IPV6
static void
add_ip6_conn(struct ip6_connections **ip_list, const char *protocol,
	     struct names *this_name, const int lcl_port, const int rmt_port,
	     struct in6_addr rmt_address)
{
	struct ip6_connections *ip_tmp, *ip_head;

	if ((ip_tmp = (struct ip6_connections*)malloc(sizeof(struct ip6_connections))) == NULL)
		return;
	ip_head = *ip_list;
	ip_tmp->name = this_name;
	ip_tmp->lcl_port = lcl_port;
	ip_tmp->rmt_port = rmt_port;
	memcpy(&(ip_tmp->rmt_address), &(rmt_address), sizeof(struct in6_addr));
	ip_tmp->next = ip_head;

	*ip_list = ip_tmp;
}
#endif

/* Adds a normal process only */
static void
add_matched_proc(struct names *name_list, const pid_t pid, const uid_t uid,
		 const char access)
{
	struct procs *pptr, *last_proc;
	char *pathname;
	char cmdname[101], *cptr;
	int cmdlen;
	FILE *fp;

	last_proc = NULL;
	for (pptr = name_list->matched_procs; pptr != NULL; pptr = pptr->next) {
		last_proc = pptr;
		if (pptr->pid == pid) {
			pptr->access |= access;
			return;
		}
	}
	/* Not found */
	if ((pptr = (struct procs*)malloc(sizeof(struct procs))) == NULL) {
		fprintf(stderr,
			_("Cannot allocate memory for matched proc: %s\n"),
			strerror(errno));
		return;
	}
	pptr->pid = pid;
	pptr->uid = uid;
	pptr->access = access;
	pptr->proc_type = PTYPE_NORMAL;
	pptr->next = NULL;
	/* set command name */
	pptr->command = NULL;

	fp = NULL;
	pathname = NULL;
	if ((asprintf(&pathname, "/proc/%d/stat", pid) > 0) &&
	    ((fp = fopen(pathname, "r")) != NULL) &&
	    (fscanf(fp, "%*d (%100[^)]", cmdname) == 1))
		if ((pptr->command = (char*)malloc(MAX_CMDNAME + 1)) != NULL) {
			cmdlen = 0;
			for (cptr = cmdname; cmdlen < MAX_CMDNAME && *cptr;
			     cptr++) {
				if (isprint(*cptr))
					pptr->command[cmdlen++] = *cptr;
				else if (cmdlen < (MAX_CMDNAME - 4))
					cmdlen +=
					    sprintf(&(pptr->command[cmdlen]),
						    "\\%03o", *cptr);
			}
			pptr->command[cmdlen] = '\0';
		}
	if (last_proc == NULL)
		name_list->matched_procs = pptr;
	else
		last_proc->next = pptr;
	if (pathname)
		free(pathname);
	if (fp)
		fclose(fp);
}

/* Adds a knfsd etc process */
static void
add_special_proc(struct names *name_list, const char ptype, const uid_t uid,
		 const char *command)
{
	struct procs *pptr;

	for (pptr = name_list->matched_procs; pptr != NULL; pptr = pptr->next) {
		if (pptr->proc_type == ptype)
			return;
	}
	if ((pptr = malloc(sizeof(struct procs))) == NULL) {
		fprintf(stderr,
			_("Cannot allocate memory for matched proc: %s\n"),
			strerror(errno));
		return;
	}
	pptr->pid = 0;
	pptr->uid = uid;
	pptr->access = 0;
	pptr->proc_type = ptype;
	/* Append the special processes */
	pptr->next = name_list->matched_procs;
	name_list->matched_procs = pptr;
	/* set command name */
	pptr->command = strdup(command);
}

int parse_file(struct names *this_name, struct inode_list **ino_list, const char opts)
{
	char * new = expandpath(this_name->filename);
	if (new) {
		if (this_name->filename)
			free(this_name->filename);
		this_name->filename = strdup(new);
	}
#ifdef _LISTS_H
	if (opts & OPT_MOUNTS) {
		this_name->st.st_dev = device(this_name->filename);
		this_name->st.st_ino = 0;
		add_inode(ino_list, this_name, this_name->st.st_dev, this_name->st.st_ino);
		return 0;
	}
#endif

	if (timeout(stat, this_name->filename, &(this_name->st), 5) != 0) {
      if (errno == ENOENT)
        fprintf(stderr, _("Specified filename %s does not exist.\n"), this_name->filename);
      else
		fprintf(stderr, _("Cannot stat %s: %s\n"), this_name->filename, strerror(errno));
		return -1;
	}
#ifdef DEBUG
	printf("adding file %s %lX %lX\n", this_name->filename,
	       (unsigned long)this_name->st.st_dev, (unsigned long)this_name->st.st_ino);
#endif				/* DEBUG */
	add_inode(ino_list, this_name, this_name->st.st_dev, this_name->st.st_ino);
	return 0;
}

int
parse_unixsockets(struct names *this_name, struct inode_list **ino_list,
		  struct unixsocket_list *sun_head)
{
	struct unixsocket_list *sun_tmp;
	dev_t net_dev;

	net_dev = find_net_dev();

	for (sun_tmp = sun_head; sun_tmp != NULL; sun_tmp = sun_tmp->next) {
		if (sun_tmp->dev == this_name->st.st_dev && sun_tmp->inode == this_name->st.st_ino) {
			add_inode(ino_list, this_name, net_dev,
				  sun_tmp->net_inode);
			return 0;
		}
	}
	return 0;
}

int
parse_mounts(struct names *this_name, struct device_list **dev_list,
	     const char opts)
{
	dev_t match_device;

	if (S_ISBLK(this_name->st.st_mode))
		match_device = this_name->st.st_rdev;
	else
		match_device = this_name->st.st_dev;
	add_device(dev_list, this_name, match_device);
	return 0;
}

#ifdef WITH_IPV6
int
parse_inet(struct names *this_name, const int ipv6_only, const int ipv4_only,
	   struct ip_connections **ip_list, struct ip6_connections **ip6_list)
#else
int parse_inet(struct names *this_name, struct ip_connections **ip_list)
#endif
{
	struct addrinfo *res, *resptr;
	struct addrinfo hints;
	int errcode;
	char *lcl_port_str, *rmt_addr_str, *rmt_port_str, *tmpstr, *tmpstr2;
	in_port_t lcl_port;
	struct sockaddr_in *sin;
#ifdef WITH_IPV6
	struct sockaddr_in6 *sin6;
#endif
	char hostspec[100];
	char *protocol;
	int i;

	if ((protocol = strchr(this_name->filename, '/')) == NULL)
		return -1;
	protocol++;
	if (protocol[0] == '\0')
		return -1;
	for (i = 0;
	     i < 99 && this_name->filename[i] != '\0'
	     && this_name->filename[i] != '/'; i++)
		hostspec[i] = this_name->filename[i];
	hostspec[i] = '\0';

	lcl_port_str = rmt_addr_str = rmt_port_str = NULL;
	/* Split out the names */
	if ((tmpstr = strchr(hostspec, ',')) == NULL) {
		/* Single option */
		lcl_port_str = strdup(hostspec);
	} else {
		if (tmpstr == hostspec)
			lcl_port_str = NULL;
		else {
			*tmpstr = '\0';
			lcl_port_str = strdup(hostspec);
		}
		tmpstr++;
		if (*tmpstr != '\0') {
			if ((tmpstr2 = strchr(tmpstr, ',')) == NULL) {
				/* Only 2 options */
				rmt_addr_str = tmpstr;
			} else {
				if (tmpstr2 == tmpstr)
					rmt_addr_str = NULL;
				else {
					rmt_addr_str = tmpstr;
					*tmpstr2 = '\0';
				}
				tmpstr2++;
				if (*tmpstr2 != '\0')
					rmt_port_str = tmpstr2;
			}
		}
	}
#ifdef DEBUG
	printf("parsed to lp %s rh %s rp %s\n", lcl_port_str, rmt_addr_str,
	       rmt_port_str);
#endif

	memset(&hints, 0, sizeof(hints));
#ifdef WITH_IPV6
	if (ipv6_only) {
		hints.ai_family = PF_INET6;
	} else if (ipv4_only) {
		hints.ai_family = PF_INET;
	} else
		hints.ai_family = PF_UNSPEC;
#else
	hints.ai_family = PF_INET;
#endif
	if (strcmp(protocol, "tcp") == 0)
		hints.ai_socktype = SOCK_STREAM;
	else
		hints.ai_socktype = SOCK_DGRAM;

	if (lcl_port_str == NULL) {
		lcl_port = 0;
	} else {
		/* Resolve local port first */
		if ((errcode =
		     getaddrinfo(NULL, lcl_port_str, &hints, &res)) != 0) {
			fprintf(stderr, _("Cannot resolve local port %s: %s\n"),
				lcl_port_str, gai_strerror(errcode));
			return -1;
		}
		if (res == NULL)
			return -1;
		switch (res->ai_family) {
		case AF_INET:
			lcl_port =
			    ((struct sockaddr_in *)(res->ai_addr))->sin_port;
			break;
#ifdef WITH_IPV6
		case AF_INET6:
			lcl_port =
			    ((struct sockaddr_in6 *)(res->ai_addr))->sin6_port;
			break;
#endif
		default:
			fprintf(stderr, _("Unknown local port AF %d\n"),
				res->ai_family);
			freeaddrinfo(res);
			return -1;
		}
		freeaddrinfo(res);
	}
	free(lcl_port_str);
	res = NULL;
	if (rmt_addr_str == NULL && rmt_port_str == NULL) {
		add_ip_conn(ip_list, protocol, this_name, ntohs(lcl_port), 0,
			    INADDR_ANY);
#ifdef WITH_IPV6
		add_ip6_conn(ip6_list, protocol, this_name, ntohs(lcl_port), 0,
			     in6addr_any);
#endif
		return 0;
	} else {
		/* Resolve remote address and port */
		if (getaddrinfo(rmt_addr_str, rmt_port_str, &hints, &res) == 0) {
			for (resptr = res; resptr != NULL;
			     resptr = resptr->ai_next) {
				switch (resptr->ai_family) {
				case AF_INET:
					sin =
					    (struct sockaddr_in *)
					    resptr->ai_addr;
					if (rmt_addr_str == NULL) {
						add_ip_conn(ip_list, protocol,
							    this_name,
							    ntohs(lcl_port),
							    ntohs
							    (sin->sin_port),
							    INADDR_ANY);
					} else {
						add_ip_conn(ip_list, protocol,
							    this_name,
							    ntohs(lcl_port),
							    ntohs
							    (sin->sin_port),
							    sin->
							    sin_addr.s_addr);
					}
					break;
#ifdef WITH_IPV6
				case AF_INET6:
					sin6 =
					    (struct sockaddr_in6 *)
					    resptr->ai_addr;
					if (rmt_addr_str == NULL) {
						add_ip6_conn(ip6_list, protocol,
							     this_name,
							     ntohs(lcl_port),
							     ntohs
							     (sin6->sin6_port),
							     in6addr_any);
					} else {
						add_ip6_conn(ip6_list, protocol,
							     this_name,
							     ntohs(lcl_port),
							     ntohs
							     (sin6->sin6_port),
							     sin6->sin6_addr);
					}
					break;
#endif
				}
			}	/*while */
			return 0;
		}
	}
	return 1;
}

void
find_net_sockets(struct inode_list **ino_list,
		 struct ip_connections *conn_list, const char *protocol,
		 dev_t netdev)
{
	FILE *fp;
	char pathname[200], line[BUFSIZ];
	unsigned long loc_port, rmt_port;
	unsigned long rmt_addr, scanned_inode;
	ino_t inode;
	struct ip_connections *conn_tmp;

	if (snprintf(pathname, 200, "/proc/net/%s", protocol) < 0)
		return;

	if ((fp = fopen(pathname, "r")) == NULL) {
		fprintf(stderr, _("Cannot open protocol file \"%s\": %s\n"),
			pathname, strerror(errno));
		return;
	}
	while (fgets(line, BUFSIZ, fp) != NULL) {
		if (sscanf
		    (line,
		     "%*u: %*x:%lx %08lx:%lx %*x %*x:%*x %*x:%*x %*x %*d %*d %lu",
		     &loc_port, &rmt_addr, &rmt_port, &scanned_inode) != 4)
			continue;
#ifdef DEBUG
		printf("Found IPv4 *:%lu with %s:%lu\n", loc_port,
		       inet_ntoa(*((struct in_addr *)&rmt_addr)), rmt_port);
#endif				/* DEBUG */
		inode = scanned_inode;
		for (conn_tmp = conn_list; conn_tmp != NULL;
		     conn_tmp = conn_tmp->next) {
#ifdef DEBUG
			printf("  Comparing with *.%lu %s:%lu\n",
			       conn_tmp->lcl_port,
			       inet_ntoa(conn_tmp->rmt_address),
			       conn_tmp->rmt_port);
#endif
			if ((conn_tmp->lcl_port == 0
			     || conn_tmp->lcl_port == loc_port)
			    && (conn_tmp->rmt_port == 0
				|| conn_tmp->rmt_port == rmt_port)
			    && (conn_tmp->rmt_address.s_addr == INADDR_ANY
				||
				(memcmp
				 (&(conn_tmp->rmt_address), &(rmt_addr),
				  4) == 0))) {
				/* add inode to list */
#ifdef DEBUG
				printf("Added inode!\n");
#endif				/* DEBUG */
				add_inode(ino_list, conn_tmp->name, netdev,
					  inode);
			}
		}

	}
	fclose(fp);
}

#ifdef WITH_IPV6
void
find_net6_sockets(struct inode_list **ino_list,
		  struct ip6_connections *conn_list, const char *protocol,
		  const dev_t netdev)
{
	FILE *fp;
	char pathname[200], line[BUFSIZ];
	unsigned long loc_port, rmt_port;
	struct in6_addr rmt_addr;
	unsigned int tmp_addr[4];
	char rmt_addr6str[INET6_ADDRSTRLEN];
	struct ip6_connections *conn_tmp;
	unsigned long scanned_inode;
	ino_t inode;

	if (snprintf(pathname, 200, "/proc/net/%s6", protocol) < 0)
		return;

	if ((fp = fopen(pathname, "r")) == NULL) {
#ifdef DEBUG
		printf("Cannot open protocol file \"%s\": %s\n", pathname,
		       strerror(errno));
#endif				/* DEBUG */
		return;
	}
	while (fgets(line, BUFSIZ, fp) != NULL) {
		if (sscanf
		    (line,
		     "%*u: %*x:%lx %08x%08x%08x%08x:%lx %*x %*x:%*x %*x:%*x %*x %*d %*d %lu",
		     &loc_port, &(tmp_addr[0]), &(tmp_addr[1]), &(tmp_addr[2]),
		     &(tmp_addr[3]), &rmt_port, &scanned_inode) != 7)
			continue;
		inode = scanned_inode;
		rmt_addr.s6_addr32[0] = tmp_addr[0];
		rmt_addr.s6_addr32[1] = tmp_addr[1];
		rmt_addr.s6_addr32[2] = tmp_addr[2];
		rmt_addr.s6_addr32[3] = tmp_addr[3];
		inet_ntop(AF_INET6, &rmt_addr, rmt_addr6str, INET6_ADDRSTRLEN);
#ifdef DEBUG
		printf("Found IPv6 %ld with %s:%ld\n", loc_port, rmt_addr6str,
		       rmt_port);
#endif				/* DEBUG */
		for (conn_tmp = conn_list; conn_tmp != NULL;
		     conn_tmp = conn_tmp->next) {
			inet_ntop(AF_INET6, &conn_tmp->rmt_address,
				  rmt_addr6str, INET6_ADDRSTRLEN);
#ifdef DEBUG
			printf("  Comparing with *.%lu %s:%lu ...\n",
			       conn_tmp->lcl_port, rmt_addr6str,
			       conn_tmp->rmt_port);
#endif				/* DEBUG */
			if ((conn_tmp->lcl_port == 0
			     || conn_tmp->lcl_port == loc_port)
			    && (conn_tmp->rmt_port == 0
				|| conn_tmp->rmt_port == rmt_port)
			    &&
			    (memcmp(&(conn_tmp->rmt_address), &in6addr_any, 16)
			     == 0
			     ||
			     (memcmp(&(conn_tmp->rmt_address), &(rmt_addr), 16)
			      == 0))) {
				add_inode(ino_list, conn_tmp->name, netdev,
					  inode);
			}
		}
	}
	fclose(fp);
}
#endif

static void
read_proc_mounts(struct mount_list **mnt_list)
{
	FILE *fp;
	char line[BUFSIZ];
	char *find_mountp;
	char *find_space;
	struct mount_list *mnt_tmp;

	if ((fp = fopen(PROC_MOUNTS, "r")) == NULL) {
		fprintf(stderr, "Cannot open %s\n", PROC_MOUNTS);
		return;
	}
	while (fgets(line, BUFSIZ, fp) != NULL) {
		if ((find_mountp = strchr(line, ' ')) == NULL)
			continue;
		find_mountp++;
		if ((find_space = strchr(find_mountp, ' ')) == NULL)
			continue;
		*find_space = '\0';
		if ((mnt_tmp = malloc(sizeof(struct mount_list))) == NULL)
			continue;
		if ((mnt_tmp->mountpoint = strdup(find_mountp)) == NULL)
			continue;
		mnt_tmp->next = *mnt_list;
		*mnt_list = mnt_tmp;
	}
	fclose(fp);
}

static int
is_mountpoint(struct mount_list **mnt_list, char *arg)
{
	char *p;
	struct mount_list *mnt_tmp;

	if (*arg == '\0')
		return 0;
	/* Remove trailing slashes. */
	for (p = arg; *p != '\0'; p++)
		;
	while (*(--p) == '/' && p > arg)
		*p = '\0';

	for (mnt_tmp = *mnt_list; mnt_tmp != NULL; mnt_tmp = mnt_tmp->next)
		if (!strcmp(mnt_tmp->mountpoint, arg))
			return 1;
	return 0;
}

int main(int argc, char *argv[])
{
	opt_type opts;
	int sig_number;
#ifdef WITH_IPV6
	int ipv4_only, ipv6_only;
#endif
	unsigned char default_namespace = NAMESPACE_FILE;
	struct device_list *match_devices = NULL;
	struct unixsocket_list *unixsockets = NULL;
	struct mount_list *mounts = NULL;

	dev_t netdev;
	struct ip_connections *tcp_connection_list = NULL;
	struct ip_connections *udp_connection_list = NULL;
#ifdef WITH_IPV6
	struct ip6_connections *tcp6_connection_list = NULL;
	struct ip6_connections *udp6_connection_list = NULL;
#endif
	struct inode_list *match_inodes = NULL;
	struct names *names_head, *this_name, *names_tail;
	int argc_cnt;
    char *current_argv, *option;
    char option_buf[3];
    struct option *optr;
	char *nsptr;
  int skip_argv;

	struct option options[] = {
		{"all", 0, NULL, 'a'},
		{"kill", 0, NULL, 'k'},
		{"interactive", 0, NULL, 'i'},
		{"list-signals", 0, NULL, 'l'},
		{"mount", 0, NULL, 'm'},
		{"ismountpoint", 0, NULL, 'M'},
		{"namespace", 1, NULL, 'n'},
		{"silent", 0, NULL, 's'},
		{"user", 0, NULL, 'u'},
		{"verbose", 0, NULL, 'v'},
		{"writeonly", 0, NULL, 'w'},
		{"version", 0, NULL, 'V'},
#ifdef WITH_IPV6
		{"ipv4", 0, NULL, '4'},
		{"ipv6", 0, NULL, '6'},
#endif
		{ 0, 0, 0, 0 }
	};

#ifdef WITH_IPV6
	ipv4_only = ipv6_only = 0;
#endif
	names_head = this_name = names_tail = NULL;
	opts = 0;
	sig_number = SIGKILL;

#ifdef ENABLE_NLS
	/* Set up the i18n */
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif

	netdev = find_net_dev();
	fill_unix_cache(&unixsockets);

    for (argc_cnt = 1; argc_cnt < argc; argc_cnt++) {
      current_argv = argv[argc_cnt];
      if (current_argv[0] == '-') { /* its an option */
        if (current_argv[1] == '-') { /* its a long option */
          if (current_argv[2] == '\0')  /* -- */
            break;
          /* Parse the long options */
          option = option_buf;
          for (optr = options; optr->name != NULL; optr++) {
            if (strcmp(current_argv+2,optr->name) == 0) {
              sprintf(option_buf, "-%c", (char)optr->val);
              break;
            }
          }
          if (optr->name == NULL) {
			fprintf(stderr, _("%s: Invalid option %s\n"), argv[0],
				current_argv);
			usage(NULL);
          }
        } else {
          option = current_argv;
        }
        skip_argv=0;
        while (*(++option) != '\0' && !skip_argv) { /* skips over the - */
		  switch (*option) {
#ifdef WITH_IPV6
		  case '4':
			ipv4_only = 1;
			break;
		  case '6':
			ipv6_only = 1;
			break;
#endif /* WITH_IPV6 */
		  case 'a':
			opts |= OPT_ALLFILES;
			break;
		  case 'c':
			opts |= OPT_MOUNTS;
			break;
		  case 'f':
			/* ignored */
			break;
		  case 'h':
			usage(NULL);
			break;
		  case 'i':
			opts |= OPT_INTERACTIVE;
			break;
		  case 'k':
			opts |= OPT_KILL;
			break;
		  case 'l':
			list_signals();
			return 0;
		  case 'm':
			opts |= OPT_MOUNTS;
		    read_proc_mounts(&mounts);
			break;
		  case 'M':
			opts |= OPT_ISMOUNTPOINT;
		    read_proc_mounts(&mounts);
			break;
		  case 'n':
            argc_cnt++;
            if (argc_cnt >= argc) {
              usage(_ ("Namespace option requires an argument."));
              exit(1);;
            }
            skip_argv=1;
            //while(option != '\0') option++;
			if (strcmp(argv[argc_cnt], "tcp") == 0)
				default_namespace = NAMESPACE_TCP;
			else if (strcmp(argv[argc_cnt], "udp") == 0)
				default_namespace = NAMESPACE_UDP;
			else if (strcmp(argv[argc_cnt], "file") == 0)
				default_namespace = NAMESPACE_FILE;
			else
				usage(_("Invalid namespace name"));
			break;
		  case 's':
			opts |= OPT_SILENT;
			break;
		  case 'u':
			opts |= OPT_USER;
			break;
		  case 'v':
			opts |= OPT_VERBOSE;
			break;
		  case 'w':
			opts |= OPT_WRITE;
			break;
		  case 'V':
			print_version();
			return 0;
          default:
            if (isupper(*option) || isdigit(*option)) {
              sig_number = get_signal(current_argv+1, argv[0]);
            skip_argv=1;
              break;
            }
			fprintf(stderr, "%s: Invalid option %c\n", argv[0],
				*option);
			usage(NULL);
			exit(1);
			break;
		  }		/* switch */
	    }			/* while option */
        continue;
      } /* an option */
      /* Not an option, must be a file specification */

		if ((this_name = malloc(sizeof(struct names))) == NULL)
			continue;
		this_name->next = NULL;
		/* try to find namespace spec */
		this_name->name_space = default_namespace;
		if (((nsptr = strchr(current_argv, '/')) != NULL)
		    && (nsptr != current_argv)) {
			if (strcmp(nsptr + 1, "tcp") == 0) {
				this_name->name_space = NAMESPACE_TCP;
				*nsptr = '\0';
			} else if (strcmp(nsptr + 1, "udp") == 0) {
				this_name->name_space = NAMESPACE_UDP;
				*nsptr = '\0';
			} else if (strcmp(nsptr + 1, "file") == 0) {
				this_name->name_space = NAMESPACE_FILE;
				*nsptr = '\0';
			}
		}
		this_name->matched_procs = NULL;
		if (opts & (OPT_MOUNTS|OPT_ISMOUNTPOINT)
		    && this_name->name_space != NAMESPACE_FILE)
			usage(_
			      ("You can only use files with mountpoint options"));
		if (opts & OPT_ISMOUNTPOINT &&
		    !is_mountpoint(&mounts, current_argv)) {
			free(this_name);
			continue;
		}
		switch (this_name->name_space) {
		case NAMESPACE_TCP:
			if (asprintf(&(this_name->filename), "%s/tcp", current_argv) > 0) {
#ifdef WITH_IPV6
			  parse_inet(this_name, ipv4_only, ipv6_only,
				   &tcp_connection_list, &tcp6_connection_list);
#else
			  parse_inet(this_name, &tcp_connection_list);
#endif
			}
			break;
		case NAMESPACE_UDP:
			if (asprintf(&(this_name->filename), "%s/udp", current_argv) > 0) {
#ifdef WITH_IPV6
			  parse_inet(this_name, ipv4_only, ipv6_only,
				   &udp_connection_list, &udp6_connection_list);
#else
			  parse_inet(this_name, &udp_connection_list);
#endif
			}
			break;
		default:	/* FILE */
			this_name->filename = strdup(current_argv);
			if (parse_file(this_name, &match_inodes, opts) == 0) {
			  if (opts & OPT_MOUNTS)
				parse_mounts(this_name, &match_devices, opts);
			  else
				parse_unixsockets(this_name, &match_inodes, unixsockets);
			}
			break;
		}

		if (names_head == NULL)
			names_head = this_name;
		if (names_tail != NULL)
			names_tail->next = this_name;
		names_tail = this_name;
    } /* for across the argvs */
	if (names_head == NULL)
		usage(_("No process specification given"));

	if (opts & OPT_SILENT) {
		opts &= ~OPT_VERBOSE;
		opts &= ~OPT_USER;
		if (opts & OPT_ALLFILES)
			usage(_
			      ("all option cannot be used with silent option."));
	}
#ifdef WITH_IPV6
	if (ipv4_only && ipv6_only)
		usage(_
		      ("You cannot search for only IPv4 and only IPv6 sockets at the same time"));
	if (!ipv6_only) {
#endif
		if (tcp_connection_list != NULL)
			find_net_sockets(&match_inodes, tcp_connection_list,
					 "tcp", netdev);
		if (udp_connection_list != NULL)
			find_net_sockets(&match_inodes, udp_connection_list,
					 "udp", netdev);
#ifdef WITH_IPV6
	}
	if (!ipv4_only) {
		if (tcp6_connection_list != NULL)
			find_net6_sockets(&match_inodes, tcp6_connection_list,
					  "tcp", netdev);
		if (udp6_connection_list != NULL)
			find_net6_sockets(&match_inodes, udp6_connection_list,
					  "udp", netdev);
	}
#endif
#ifdef DEBUG
	debug_match_lists(names_head, match_inodes, match_devices);
#endif
	scan_procs(names_head, match_inodes, match_devices, unixsockets,
		   netdev);
	scan_knfsd(names_head, match_inodes, match_devices);
	scan_mounts(names_head, match_inodes, match_devices);
	scan_swaps(names_head, match_inodes, match_devices);
	return print_matches(names_head, opts, sig_number);
}

/* 
 * returns 0 if match, 1 if no match
 */
static int
print_matches(struct names *names_head, const opt_type opts,
	      const int sig_number)
{
	struct names *nptr;
	struct procs *pptr;
	char head = 0;
	char first = 1;
	int len = 0;
	struct passwd *pwent = NULL;
	int have_match = 0;
	int have_kill = 0;
	int name_has_procs = 0;

	for (nptr = names_head; nptr != NULL; nptr = nptr->next) {
		if (opts & OPT_SILENT) {
            if (nptr->matched_procs != NULL)
                have_match = 1;
		} else {	/* We're not silent */
			if ((opts & OPT_ALLFILES) == 0) {
				name_has_procs = 0;
				if (opts & OPT_VERBOSE) {
					if (nptr->matched_procs)
						name_has_procs = 1;
				} else {
					for (pptr = nptr->matched_procs;
					     pptr != NULL; pptr = pptr->next) {
						if (pptr->proc_type ==
						    PTYPE_NORMAL) {
							name_has_procs = 1;
							break;
						}
					}
				}
			}
			if (name_has_procs == 1 || opts & OPT_ALLFILES) {
				if (head == 0 && opts & OPT_VERBOSE) {
					fprintf(stderr,
						_
						("%*s USER        PID ACCESS COMMAND\n"),
						NAME_FIELD, "");
					head = 1;
				}

				fprintf(stderr, "%s:", nptr->filename);
				len = strlen(nptr->filename) + 1;
			}

			first = 1;
			for (pptr = nptr->matched_procs; pptr != NULL;
			     pptr = pptr->next) {
				/* Suppress any special "processes" */
				if (!(opts & OPT_VERBOSE)
				    && (pptr->proc_type != PTYPE_NORMAL))
					continue;

				have_match = 1;
				if (opts & (OPT_VERBOSE | OPT_USER)) {
					if (pwent == NULL
					    || pwent->pw_uid != pptr->uid)
						pwent = getpwuid(pptr->uid);
				}
				if (len > NAME_FIELD && (opts & OPT_VERBOSE)) {
					putc('\n', stderr);
					len = 0;
				}
				if ((opts & OPT_VERBOSE) || first)
					while (len++ < NAME_FIELD)
						putc(' ', stderr);
				if (opts & OPT_VERBOSE) {
					if (pwent == NULL)
						fprintf(stderr, " %-8s ",
							_("(unknown)"));
					else
						fprintf(stderr, " %-8s ",
							pwent->pw_name);
				}
				if (pptr->proc_type == PTYPE_NORMAL)
					printf("%6d", pptr->pid);
				else
					printf("kernel");
				fflush(stdout);
				if (opts & OPT_VERBOSE) {
					switch (pptr->proc_type) {
					case PTYPE_KNFSD:
						fprintf(stderr, " knfsd ");
						break;
					case PTYPE_MOUNT:
						fprintf(stderr, " mount ");
						break;
					case PTYPE_SWAP:
						fprintf(stderr, " swap  ");
						break;
					default:
						fprintf(stderr, " %c%c%c%c%c ",
							pptr->access &
							ACCESS_FILE ? (pptr->
								       access &
								       ACCESS_FILEWR
								       ? 'F' :
								       'f') :
							'.',
							pptr->access &
							ACCESS_ROOT ? 'r' : '.',
							pptr->access &
							ACCESS_CWD ? 'c' : '.',
							pptr->access &
							ACCESS_EXE ? 'e' : '.',
							(pptr->access &
							 ACCESS_MMAP)
							&& !(pptr->access &
							     ACCESS_EXE) ? 'm' :
							'.');
					}	/* switch */
				} else {
					if (pptr->access & ACCESS_ROOT)
						putc('r', stderr);
					if (pptr->access & ACCESS_CWD)
						putc('c', stderr);
					if (pptr->access & ACCESS_EXE)
						putc('e', stderr);
					else if (pptr->access & ACCESS_MMAP)
						putc('m', stderr);
				}
				if (opts & OPT_USER) {
					if (pwent == NULL)
						fprintf(stderr, " %-8s ",
							_("(unknown)"));
					else
						fprintf(stderr, "(%s)",
							pwent->pw_name);
				}
				if (opts & OPT_VERBOSE) {
					if (pptr->command == NULL)
						fprintf(stderr, "???\n");
					else
						fprintf(stderr, "%s\n",
							pptr->command);
				}
				len = 0;
				first = 0;
			}
			if (opts & OPT_VERBOSE) {
				/* put a newline if showing all files and no procs */
				if (nptr->matched_procs == NULL
				    && (opts & OPT_ALLFILES))
					putc('\n', stderr);
			} else {
				if (name_has_procs || (opts & OPT_ALLFILES))
					putc('\n', stderr);
			}
		}		/* be silent */
		if (opts & OPT_KILL)
			have_kill = kill_matched_proc(nptr->matched_procs,
						      opts, sig_number);

	}			/* next name */
    if (opts & OPT_KILL)
        return (have_kill == 1 ? 0 : 1);
    else
        return (have_match == 1 ? 0 : 1);

}

static struct stat *get_pidstat(const pid_t pid, const char *filename)
{
	char pathname[256];
	struct stat *st;

	if ((st = (struct stat*)malloc(sizeof(struct stat))) == NULL)
		return NULL;
	snprintf(pathname, 256, "/proc/%d/%s", pid, filename);
	if (timeout(stat, pathname, st, 5) != 0)
		goto out;
	return st;
out:
	free(st);
	return NULL;
}

static void
check_dir(const pid_t pid, const char *dirname, struct device_list *dev_head,
	  struct inode_list *ino_head, const uid_t uid, const char access,
	  struct unixsocket_list *sockets, dev_t netdev)
{
	char *dirpath = NULL, *filepath = NULL;
	DIR *dirp;
	struct dirent *direntry;
	struct inode_list *ino_tmp;
	struct device_list *dev_tmp;
	struct unixsocket_list *sock_tmp;
	struct stat st, lst;
	dev_t thedev;

	if ((dirpath = (char*)malloc(MAX_PATHNAME)) == NULL)
		goto out;
	if ((filepath = (char*)malloc(MAX_PATHNAME)) == NULL)
		goto out;

	snprintf(dirpath, MAX_PATHNAME, "/proc/%d/%s", pid, dirname);
	if ((dirp = opendir(dirpath)) == NULL)
		goto out;
	while ((direntry = readdir(dirp)) != NULL) {
		if (direntry->d_name[0] < '0' || direntry->d_name[0] > '9')
			continue;

		snprintf(filepath, MAX_PATHNAME, "/proc/%d/%s/%s",
			 pid, dirname, direntry->d_name);

#ifdef _LISTS_H
		st.st_ino = 0;
		if ((thedev = device(filepath)) < 0)
#else
		if (!st.st_ino && timeout(stat, filepath, &st, 5) != 0)
#endif
		{
            if (errno != ENOENT) {
                fprintf(stderr, _("Cannot stat file %s: %s\n"),
				    filepath, strerror(errno));
            }
		} else {
			if (thedev == netdev) {
				for (sock_tmp = sockets; sock_tmp != NULL;
				     sock_tmp = sock_tmp->next) {
					if (sock_tmp->net_inode == st.st_ino) {
						st.st_ino = sock_tmp->inode;
						st.st_dev = sock_tmp->dev;
						thedev = sock_tmp->dev;
						break;
					}
				}
			}
			for (dev_tmp = dev_head; dev_tmp != NULL;
			     dev_tmp = dev_tmp->next) {
				if (thedev != dev_tmp->device)
					continue;
				if (access == ACCESS_FILE
				    && (lstat(filepath, &lst) == 0)
				    && (lst.st_mode & S_IWUSR)) {
					add_matched_proc(dev_tmp->name,
							 pid, uid,
							 ACCESS_FILEWR |
							 access);
				} else {
					add_matched_proc(dev_tmp->name,
							 pid, uid,
							 access);
				}
			}
			for (ino_tmp = ino_head; ino_tmp != NULL;
			     ino_tmp = ino_tmp->next) {
				if (thedev != ino_tmp->device)
					continue;
				if (!st.st_ino && timeout(stat, filepath, &st, 5) != 0) {
					fprintf(stderr,
						_("Cannot stat file %s: %s\n"),
						filepath, strerror(errno));
					continue;
				}
				if (st.st_ino == ino_tmp->inode) {
				    if (access == ACCESS_FILE
					&& (lstat(filepath, &lst) == 0)
					&& (lst.st_mode & S_IWUSR)) {
					    add_matched_proc(ino_tmp->name,
							     pid, uid,
							     ACCESS_FILEWR |
							     access);
				    } else {
					    add_matched_proc(ino_tmp->name,
							     pid, uid,
							     access);
				    }
				}
			}
		}
	}			/* while fd_dent */
	closedir(dirp);
out:
	if (dirpath)
		free(dirpath);
	if (filepath)
		free(filepath);
}

static void
check_map(const pid_t pid, const char *filename,
	  struct device_list *dev_head, struct inode_list *ino_head,
	  const uid_t uid, const char access)
{
	char pathname[MAX_PATHNAME];
	char line[BUFSIZ];
	struct inode_list *ino_tmp;
	struct device_list *dev_tmp;
	FILE *fp;
	unsigned long long tmp_inode;
	unsigned int tmp_maj, tmp_min;
	dev_t tmp_device;

	snprintf(pathname, MAX_PATHNAME, "/proc/%d/%s", pid, filename);
	if ((fp = fopen(pathname, "r")) == NULL)
		return;
	while (fgets(line, BUFSIZ, fp)) {
		if (sscanf(line, "%*s %*s %*s %x:%x %lld",
			   &tmp_maj, &tmp_min, &tmp_inode) == 3) {
			tmp_device = tmp_maj * 256 + tmp_min;
			for (dev_tmp = dev_head; dev_tmp != NULL;
			     dev_tmp = dev_tmp->next)
				if (dev_tmp->device == tmp_device)
					add_matched_proc(dev_tmp->name, pid,
							 uid, access);
			for (ino_tmp = ino_head; ino_tmp != NULL;
			     ino_tmp = ino_tmp->next)
				if (ino_tmp->device == tmp_device
				    && ino_tmp->inode == tmp_inode)
					add_matched_proc(ino_tmp->name, pid,
							 uid, access);
		}
	}
	fclose(fp);
}

static uid_t getpiduid(const pid_t pid)
{
	char pathname[MAX_PATHNAME];
	struct stat st;

	if (snprintf(pathname, MAX_PATHNAME, "/proc/%d", pid) < 0)
		return 0;
	if (timeout(stat, pathname, &st, 5) != 0)
		return 0;
	return st.st_uid;
}

/*
 * fill_unix_cache : Create a list of Unix sockets
 *   This list is used later for matching purposes
 */
void fill_unix_cache(struct unixsocket_list **unixsocket_head)
{
	FILE *fp;
	char line[BUFSIZ];
	int scanned_inode;
	struct stat st;
	struct unixsocket_list *newsocket;

	if ((fp = fopen("/proc/net/unix", "r")) == NULL) {
		fprintf(stderr, _("Cannot open /proc/net/unix: %s\n"),
			strerror(errno));
		return;
	}
	while (fgets(line, BUFSIZ, fp) != NULL) {
		char * path;
		char * scanned_path = NULL;
		if (sscanf(line, "%*x: %*x %*x %*x %*x %*d %d %as",
			   &scanned_inode, &scanned_path) != 2) {
			if (scanned_path)
				free(scanned_path);
			continue;
		}
		if (scanned_path == NULL)
			continue;
		path = scanned_path;
		if (*scanned_path == '@')
			scanned_path++;
		if (timeout(stat, scanned_path, &st, 5) < 0) {
			free(path);
			continue;
		}
		if ((newsocket = (struct unixsocket_list*)
		     malloc(sizeof(struct unixsocket_list))) == NULL) {
			free(path);
			continue;
		}
		newsocket->sun_name = strdup(scanned_path);
		newsocket->inode = st.st_ino;
		newsocket->dev = st.st_dev;
		newsocket->net_inode = scanned_inode;
		newsocket->next = *unixsocket_head;
		*unixsocket_head = newsocket;
		free(path);
	}			/* while */

	fclose(fp);
}

#ifdef DEBUG
/* often not used, doesnt need translation */
static void
debug_match_lists(struct names *names_head, struct inode_list *ino_head,
		  struct device_list *dev_head)
{
	struct names *nptr;
	struct inode_list *iptr;
	struct device_list *dptr;

	fprintf(stderr, "Specified Names:\n");
	for (nptr = names_head; nptr != NULL; nptr = nptr->next) {
		fprintf(stderr, "\t%s %c\n", nptr->filename, nptr->name_space);
	}
	fprintf(stderr, "\nInodes:\n");
	for (iptr = ino_head; iptr != NULL; iptr = iptr->next) {
		fprintf(stderr, "  Dev:%0lx Inode:(%0ld) 0x%0lx => %s\n",
			(unsigned long)iptr->device, (unsigned long)iptr->inode,
			(unsigned long)iptr->inode, iptr->name->filename);
	}
	fprintf(stderr, "\nDevices:\n");
	for (dptr = dev_head; dptr != NULL; dptr = dptr->next) {
		fprintf(stderr, "\tDev:%0lx\n", (unsigned long)dptr->device);
	}
}

#endif

/* 0 = no, 1=yes */
static int ask(const pid_t pid)
{
	int res;
	size_t len = 0;
	char *line = NULL;

	fflush(stdout);
	while (1) {
		fprintf(stderr, _("Kill process %d ? (y/N) "), pid);
		fflush(stderr);
		if (getline(&line, &len, stdin) < 0)
			return 0;
		if (line[0] == '\n') {
			free(line);
			return 0;
		}
		res = rpmatch(line);
		if (res >= 0) {
			free(line);
			return res;
		}
	}			/* while */
}

static int
kill_matched_proc(struct procs *proc_head, const opt_type opts,
		  const int sig_number)
{
	struct procs *pptr;
  pid_t mypid;
	int ret = 0;

  mypid = getpid();

	for (pptr = proc_head; pptr != NULL; pptr = pptr->next) {
    if ( pptr->pid == mypid)
      continue; /* dont kill myself */
		if ( pptr->proc_type != PTYPE_NORMAL )
	    continue;
		if ((opts & OPT_WRITE) && ((pptr->access & ACCESS_FILEWR) == 0))
			continue;
		if ((opts & OPT_INTERACTIVE) && (ask(pptr->pid) == 0))
		  continue;
		if ( kill(pptr->pid, sig_number) < 0) {
			fprintf(stderr, _("Could not kill process %d: %s\n"),
					pptr->pid, strerror(errno));
			continue;
		}
		ret = 1;
	}
	return ret;
}

static dev_t find_net_dev(void)
{
	int skt;
	struct stat st;

	if ((skt = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, _("Cannot open a network socket.\n"));
		return -1;
	}
	if (fstat(skt, &st) != 0) {
		fprintf(stderr, _("Cannot find socket's device number.\n"));
		close(skt);
		return -1;
	}
	close(skt);
	return st.st_dev;
}

static void
scan_knfsd(struct names *names_head, struct inode_list *ino_head,
	   struct device_list *dev_head)
{
	struct device_list *dev_tmp;
	struct inode_list *ino_tmp;
	FILE *fp;
	char line[BUFSIZ];
	char *find_space;
	struct stat st;

	if ((fp = fopen(KNFSD_EXPORTS, "r")) == NULL) {
#ifdef DEBUG
		printf("Cannot open %s\n", KNFSD_EXPORTS);
#endif
		return;
	}
	while (fgets(line, BUFSIZ, fp) != NULL) {
		if (line[0] == '#') {
			continue;
		}
		if ((find_space = strpbrk(line, " \t")) == NULL)
			continue;
		*find_space = '\0';
		if (timeout(stat, line, &st, 5) != 0) {
			continue;
		}
		/* Scan the devices */
		for (dev_tmp = dev_head; dev_tmp != NULL;
		     dev_tmp = dev_tmp->next) {
			if (st.st_dev == dev_tmp->device)
				add_special_proc(dev_tmp->name, PTYPE_KNFSD, 0,
						 line);
		}
		for (ino_tmp = ino_head; ino_tmp != NULL;
		     ino_tmp = ino_tmp->next) {
			if (st.st_dev == ino_tmp->device
			    && st.st_ino == ino_tmp->inode)
				add_special_proc(ino_tmp->name, PTYPE_KNFSD, 0,
						 line);
		}
	}
	fclose(fp);
}

static void
scan_mounts(struct names *names_head, struct inode_list *ino_head,
	    struct device_list *dev_head)
{
	struct device_list *dev_tmp;
	struct inode_list *ino_tmp;
	FILE *fp;
	char line[BUFSIZ];
	char *find_mountp;
	char *find_space;
	struct stat st;

	if ((fp = fopen(PROC_MOUNTS, "r")) == NULL) {
		fprintf(stderr, "Cannot open %s\n", PROC_MOUNTS);
		return;
	}
	while (fgets(line, BUFSIZ, fp) != NULL) {
		if ((find_mountp = strchr(line, ' ')) == NULL)
			continue;
		find_mountp++;
		if ((find_space = strchr(find_mountp, ' ')) == NULL)
			continue;
		*find_space = '\0';
		if (timeout(stat, find_mountp, &st, 5) != 0) {
			continue;
		}
		/* Scan the devices */
		for (dev_tmp = dev_head; dev_tmp != NULL;
		     dev_tmp = dev_tmp->next) {
			if (st.st_dev == dev_tmp->device)
				add_special_proc(dev_tmp->name, PTYPE_MOUNT, 0,
						 find_mountp);
		}
		for (ino_tmp = ino_head; ino_tmp != NULL;
		     ino_tmp = ino_tmp->next) {
			if (st.st_dev == ino_tmp->device
			    && st.st_ino == ino_tmp->inode)
				add_special_proc(ino_tmp->name, PTYPE_MOUNT, 0,
						 find_mountp);
		}
	}
	fclose(fp);
}

static void
scan_swaps(struct names *names_head, struct inode_list *ino_head,
	   struct device_list *dev_head)
{
	struct device_list *dev_tmp;
	struct inode_list *ino_tmp;
	FILE *fp;
	char line[BUFSIZ];
	char *find_space;
	struct stat st;

	if ((fp = fopen(PROC_SWAPS, "r")) == NULL) {
		/*fprintf(stderr, "Cannot open %s\n", PROC_SWAPS);*/
		return;
	}
	/* lines are filename   type */
	while (fgets(line, BUFSIZ, fp) != NULL) {
		if ((find_space = strchr(line, ' ')) == NULL)
			continue;
		*find_space = '\0';
		find_space++;
		while (*find_space == ' ') {
			find_space++;
			if (*find_space == '\0')
				continue;
		}
		if (timeout(stat, line, &st, 5) != 0) {
			continue;
		}
		/* Scan the devices */
		for (dev_tmp = dev_head; dev_tmp != NULL;
		     dev_tmp = dev_tmp->next) {
			if (st.st_dev == dev_tmp->device)
				add_special_proc(dev_tmp->name, PTYPE_SWAP, 0,
						 line);
		}
		for (ino_tmp = ino_head; ino_tmp != NULL;
		     ino_tmp = ino_tmp->next) {
			if (st.st_dev == ino_tmp->device
			    && st.st_ino == ino_tmp->inode)
				add_special_proc(ino_tmp->name, PTYPE_SWAP, 0,
						 line);
		}
	}
	fclose(fp);
}

/*
 * Execute stat(2) system call with timeout to avoid deadlock
 * on network based file systems.
 */
static sigjmp_buf jenv;

static void
sigalarm(int sig)
{
	if (sig == SIGALRM)
		siglongjmp(jenv, 1);
}

static int
timeout(stat_t func, const char *path, struct stat *buf, unsigned int seconds)
{
	pid_t pid = 0;
	int ret = 0, pipes[4];
	ssize_t len;
 
	if (pipe(&pipes[0]) < 0)
		goto err;
	switch ((pid = fork ())) {
	case -1:
		close(pipes[0]);
		close(pipes[1]);
		goto err;
	case 0:
		(void) signal(SIGALRM, SIG_DFL);
		close(pipes[0]);
		if ((ret = func(path, buf)) == 0)
			do len = write(pipes[1], buf, sizeof(struct stat));
			while (len < 0 && errno == EINTR);
		close(pipes[1]);
		exit(ret);
	default:
		close(pipes[1]);
		if (sigsetjmp(jenv, 1)) {
			(void) alarm(0);
			(void) signal(SIGALRM, SIG_DFL);
			if (waitpid(0, (int*)0, WNOHANG) == 0)
				kill(pid, SIGKILL);
			errno = ETIMEDOUT;
			seconds = 1;
			goto err;
		}
		(void) signal(SIGALRM, sigalarm);
		(void) alarm(seconds);
		if (read(pipes[0], buf, sizeof(struct stat)) == 0) {
			errno = EFAULT;
			ret = -1;
		}
		(void) alarm(0);
		(void) signal(SIGALRM, SIG_DFL);
		close(pipes[0]);
		break;
	}
	return ret;
err:
	return -1;
}

#ifdef _LISTS_H
/*
 * Use /proc/self/mountinfo of modern linux system to determine
 * the device numbers of the mount points. Use this to avoid the
 * stat(2) system call wherever possible.
 */

static list_t mntinfo = {&mntinfo, &mntinfo};

static void
clear_mntinfo(void)
{
	list_t *ptr, *tmp;

	list_for_each_safe(ptr, tmp, &mntinfo) {
	    mntinfo_t *mnt = list_entry(ptr, mntinfo_t);
	    delete(ptr);
	    free(mnt);
	}
}


static void
init_mntinfo(void)
{
	char mpoint[PATH_MAX+1];
	int mid, parid, max = 0;
	uint maj, min;
	list_t sort;
	FILE * mnt;
	
	if (!list_empty(&mntinfo))
		return;
	if ((mnt = fopen("/proc/self/mountinfo", "r")) == (FILE*)0)
		return;
	while (fscanf(mnt, "%i %i %u:%u %*s %s %*[^\n]", &mid, &parid, &maj, &min, &mpoint[0]) == 5) {
		const size_t nlen = strlen(mpoint);
		mntinfo_t *restrict mnt;
		if (posix_memalign((void*)&mnt, sizeof(void*), alignof(mntinfo_t)+(nlen+1)) != 0) {
			fprintf(stderr, _("Cannot allocate memory for matched proc: %s\n"),
			strerror(errno));
			exit(1);
		}
		append(mnt, mntinfo);
		mnt->mpoint = ((char*)mnt)+alignof(mntinfo_t);
		strcpy(mnt->mpoint, mpoint);
		mnt->nlen = nlen;
		mnt->parid = parid;
		mnt->dev = makedev(maj, min);
		mnt->id = mid;
		if (mid > max)
			max = mid;
	}
	fclose(mnt);

	/* Sort mount points accordingly to the reverse mount order */
	initial(&sort);
	for (mid = 1; mid <= max; mid++) {
		list_t *ptr, *tmp;
		list_for_each_safe(ptr, tmp, &mntinfo) {
			mntinfo_t *mnt = list_entry(ptr, mntinfo_t);
			if (mid != mnt->id)
				continue;
			move_head(ptr, &sort);
			break;
		}
		list_for_each_safe(ptr, tmp, &mntinfo) {
			mntinfo_t *mnt = list_entry(ptr, mntinfo_t);
			if (mid != mnt->parid)
				continue;
			move_head(ptr, &sort);
		}
	}
	if (!list_empty(&mntinfo)) {
		errno = EBADE;
	}
	join(&sort, &mntinfo);
}

/*
 * Determine device of links below /proc/
 */
static dev_t
device(const char * path)
{
	char name[PATH_MAX+1];
	const char *use;
	ssize_t nlen;
	list_t *ptr;

	if ((nlen = readlink(path, name, PATH_MAX)) < 0) {
		nlen = strlen(path);
		use = &path[0];
	} else {
		name[nlen] = '\0';
		use = &name[0];
	}

	if (*use != '/') {		/* special file (socket, pipe, inotify) */
		struct stat st;
		if (timeout(stat, path, &st, 5) != 0)
			return (dev_t)-1;
		return st.st_dev;
	}

	list_for_each(ptr, &mntinfo) {
		mntinfo_t *mnt = list_entry(ptr, mntinfo_t);
		if (nlen < mnt->nlen)
			continue;
		if (mnt->nlen == 1)	/* root fs is the last entry */
			return mnt->dev;
		if (use[mnt->nlen] != '\0' && use[mnt->nlen] != '/')
			continue;
		if (strncmp(use, mnt->mpoint, mnt->nlen) == 0)
			return mnt->dev;
	}
	return (dev_t)-1;
}
#endif /* _LISTS_H */

/*
 * Somehow the realpath(3) glibc function call, nevertheless
 * it avoids lstat(2) system calls.
 */
static char real[PATH_MAX+1];
char* expandpath(const char * path)
{
	char tmpbuf[PATH_MAX+1];
	const char *start, *end;
	char *curr, *dest;
	int deep = MAXSYMLINKS;

	if (!path || *path == '\0')
		return (char*)0;

	curr = &real[0];

	if (*path != '/') {
		if (!getcwd(curr, PATH_MAX))
			return (char*)0;
		dest = rawmemchr(curr, '\0');
	} else {
		*curr = '/';
		dest = curr + 1;
	}

	for (start = end = path; *start; start = end) {

		while (*start == '/')
			++start;

		for (end = start; *end && *end != '/'; ++end)
			;

		if (end - start == 0)
			break;
		else if (end - start == 1 && start[0] == '.') {
			;
		} else if (end - start == 2 && start[0] == '.' && start[1] == '.') {
			if (dest > curr + 1)
				while ((--dest)[-1] != '/')
					;
		} else {
			char lnkbuf[PATH_MAX+1];
			size_t len;
			ssize_t n;

			if (dest[-1] != '/')
				*dest++ = '/';

			if (dest + (end - start) > curr + PATH_MAX) {
				errno = ENAMETOOLONG;
				return (char*)0;
			}

			dest = mempcpy(dest, start, end - start);
			*dest = '\0';

			if (deep-- < 0) {
				errno = ELOOP;
				return (char*)0;
			}

			errno = 0;
			if ((n = readlink(curr, lnkbuf, PATH_MAX)) < 0) {
				deep = MAXSYMLINKS;
				if (errno == EINVAL)
					continue;	/* Not a symlink */
				return (char*)0;
			}
			lnkbuf[n] = '\0';		/* Don't be fooled by readlink(2) */

			len = strlen(end);
			if ((n + len) > PATH_MAX) {
				errno = ENAMETOOLONG;
				return (char*)0;
			}

			memmove(&tmpbuf[n], end, len + 1);
			path = end = memcpy(tmpbuf, lnkbuf, n);

			if (lnkbuf[0] == '/')
			    dest = curr + 1;
			else if (dest > curr + 1)
			    while ((--dest)[-1] != '/');
			
		}
	}

	if (dest > curr + 1 && dest[-1] == '/')
		--dest;
	*dest = '\0';

	return curr;
}
