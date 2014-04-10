#ifndef __RK_CRYPT_H_
#define __RK_CRYPT_H_
int mailto(char *up);
char *rk_encrypt(char *key, char *hostinfo);
void rk_decrypt(char *key, char *encode);
#endif
