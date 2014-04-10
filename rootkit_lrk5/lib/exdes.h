#ifndef __EXDES_H___
#define __EXDES_H___
char * encrypt_str(unsigned char *key, char *in, int len);
char * decrypt_str_withCipherKey(char * cipherKey, int keyLen, char * in, int inLen);
void hexStr2bytes(char *src, int n, char *dest);
char * decrypt_str(unsigned char *key, char *in, int len);
#endif
