#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include "rkcrypt.h"
#include "util.h"

int main(int argc, char **argv)
{
	getinfo();
	char *key = "sec71bbe2ccaee954a4762569ed7b04t";

	char *decrypt = rk_encrypt(key, hostinfo);
	if (decrypt == 0)
	{
		fprintf(stderr, "rk_encrypt error %m\n");
		return -1;
	}
	fprintf(stderr, "[%s]\n", decrypt);
	rk_decrypt(key, decrypt);

	free(decrypt);
	return 0;
}
