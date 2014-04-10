#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

#include "exdes.h"
#include "rkcrypt.h"

unsigned char privateKey[] = {
                            0x8b, 0xc5, 0x1e,
                            0x31, 0x06,
                            0x27, 0xfc, 0x6e};
#define KEY_MAX_LEN 32
/*可输入的密码明文最大长度*/
#define PASSWD_MAX_LEN 32

char *rk_encrypt(char *key, char *srcinfo)
{
    int maxLen = KEY_MAX_LEN;
    int ret;
    char *plainKeyText = 0;
    int bytesKeyLen = 8;
    unsigned char plainKeyBytes[8];
    char plainText[PASSWD_MAX_LEN+1];
    char * cipherText;
    
	plainKeyText = (char *)decrypt_str(privateKey, key, maxLen);
	if ((plainKeyText == 0) ||(int)strlen(plainKeyText) != 2* bytesKeyLen) 
	{
		printf("The cipher key you set is bad, try again please\n");
		return 0;
	} 
	else 
		hexStr2bytes(plainKeyText, 2*bytesKeyLen, (char *)plainKeyBytes);

	snprintf(plainText, sizeof(plainText), "%s|", srcinfo);
    cipherText = (char *)encrypt_str(plainKeyBytes, plainText, strlen(plainText));

    if (plainKeyText != 0) 
        free(plainKeyText);

    if (cipherText == 0) 
	{
        printf("Failed in encryption\n");
		return 0;
    } 
    
	return cipherText;
}

void rk_decrypt(char *key, char *encode)
{
	char *dst = (char *) decrypt_str_withCipherKey(key, strlen(key), encode, strlen(encode));
	if (dst)
	{
		char *t = strchr(dst, '|');
		if (t)
		{
			*t = 0x0;
			fprintf(stdout, "[%s %s]\n", dst, ++t);
		}
		else
			fprintf(stdout, "[%s]\n", dst);
		free(dst);
		return;
	}
	fprintf(stderr, "rk_decrypt error %m\n");
	return;
}

