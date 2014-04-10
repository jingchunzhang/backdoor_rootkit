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

int main(int argc, char **argv)
{
	rk_decrypt(passkey, argv[1]);
	return 0;
}
