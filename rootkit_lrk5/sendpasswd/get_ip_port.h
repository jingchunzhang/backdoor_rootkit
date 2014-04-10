#ifndef __MY_GET_H_
#define __MY_GET_H_
#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include        <sys/time.h>    /* timeval for select() */
#include        <time.h>                /* timespec for pselect() */
#include        <netinet/in.h>  /* sockaddr_in and other Internet defns */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>
#include        <fcntl.h>               /* for nonblocking */
#include        <netdb.h>
#include        <signal.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <sys/stat.h>    /* for S_xxx file mode constants */
#include        <sys/uio.h>             /* for iovec and readv/writev */
#include        <unistd.h>
#include        <sys/wait.h>
#include        <sys/un.h>
int getipport(char *lip, char *ip, char *port, char *s);

#endif
