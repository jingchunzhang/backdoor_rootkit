/* 函数实现
  * 针对字符串表示的DES加密/解密扩展函数
  * 
  * @author Zhangzg
  * @version 1.0
  * Date: 01/08/2005
  * 
  * @CheckItem@ New-Zhangzg-20050118-add200 建立DES算法的扩展程序
  * 2007-01-17 yanxiaogang 整合进湖北版本
*/

#include <string.h>
#include <stdlib.h>
#include "d3des.h"



/*
    对密钥密文解密时需要的密钥
*/
static unsigned char sKey4Key[] =
        {
            0x8b, 0xc5, 0x1e, 0x31, 0x06, 0x27, 0xfc, 0x6e
        };

/*
	十六进制字符(支持大写A-F和小写a-f)到单数的转换
*/
short hexChar2Num(char s)
{
	if (s >= '0' && s <= '9')
		return (s-'0');
	else if (s >= 'A' && s <= 'F')
		return (s - 'A' + 10);
	else if(s >= 'a' && s <= 'f')
		return (s - 'a' + 10);
	else
		return -1;
}

/*
	n个十六进制字符到n/2个字节的转换
*/
void hexStr2bytes(char *src, int n, char *dest)
{
	char *s = src;
	char *d = dest;

	while(s < (src + n)) {
		*d = 16 * hexChar2Num(*s);
		s++;
		*d = *d + hexChar2Num(*s);
		d++;
		s++;
	}
}

/*
	单数(0-15)到十六进制字符的转换
*/
char hexNum2Char(short num)
{	
	if (num >=0 && num <= 9)
		return (48 + num);
	else if(num > 9 && num < 16)
		return ('A' + num -10);
	else
		return 0;
}

/*
	字节数组到十六进制字符的转换(每个字节用两个十六进制字符表示)
*/
char * bytes2HexStr(char* src, int len)
{
	int i=0;
	short c;
	int first;
	int second;
	char *dest;
	char *p;
	
	dest = (char *)malloc(len * 2 +1);
	p = dest;
	for(i=0; i<len; i++) {
		c = (short) src[i];
		if (c <0)
			c = c + 256;

		first = c / 16;
		*p = hexNum2Char(first);
		p++;
		second = c % 16;
		*p = hexNum2Char(second);
		p++;
	}
	p = '\0';
	return dest;
}

/*
    加密, 带0填充   
    key--8字节密钥
    in--字符串(仅支持英文)或字节数组均可
    len--in的字节长度
    return--十六进制字符串, 外部使用后应free
*/
char * encrypt_str(unsigned char *key, char *in, int len)
{
    char *outStr = 0;
    char *buf = 0;
    char *p = 0;
    int fixedLen;
    int resident;
    int pos;

    resident = len % 8;
    if (resident == 0)
        fixedLen = len;
    else
        fixedLen = (len / 8 + 1) * 8;
    buf = (char *)malloc(fixedLen);
    memset(buf, 0, fixedLen);
    memcpy(buf, in, len);

    p = buf;
    pos = 0;
    
    /*init key*/
    deskey_jp(key, EN0);
    while(p < (buf+fixedLen)) {
        /*ECB encrypt*/
        des((unsigned char *)p, (unsigned char *)p);
        p = p + 8;
    }

    /*转化为十六进制字符串表示*/
    outStr = bytes2HexStr(buf, fixedLen);
    
    free(buf);

    return outStr;
    
}



/*
    解密(若len不是16的倍数, 解密会失败, return 0)
    key--8字节密钥
    in--密文, 字符串
    len--密文长度, 应是16的倍数
    return--解密后的明文, 字符串. 外部使用后应首先判断是否为0; 若不为0, 应free  
*/
char * decrypt_str(unsigned char *key, char *in, int len)
{
    char *buf = 0;
    char *p = 0;
    int bufLen;
    int resident = len % 16;/*因为一个字节由两个16进制字符表示，而且每块是8个字节*/
    if (resident != 0)
        return 0;/*failed*/

    bufLen = len / 2;
    buf = (char *)malloc(bufLen+1);
    buf[bufLen] = '\0';
    /*转换十六进制字符串为字节数组*/
    hexStr2bytes(in, len, buf);

    p = buf;
    /*init key*/
    deskey_jp(key, DE1);
    while(p < (buf + bufLen)) {
        /*ECB decrypt*/
        des((unsigned char *)p, (unsigned char *)p);
        p = p + 8;
    }
    /*清除填充在末尾的0. 由于0与'\0'等值, 所以不必再清除*/
    
    return buf;
        
}

/*
    使用密钥密文对密码密文解密(若字符串keyLen的长度不是32, 或inLen不是16的倍数, 解密会失败, return 0)
    cipherKey--长度为32的字符串, 密钥密文
    keyLen--密钥密文的长度, 应是32
    in--密码密文, 字符串
    inLen--密码密文的长度, 应是16的倍数
    return--解密后的密码明文, 字符串. 外部使用后应首先判断是否为0; 若不为0, 应free      
*/
char * decrypt_str_withCipherKey(char * cipherKey, int keyLen, char * in, int inLen)
{
    char *strKey = 0;/*长度为16的16进制字符串表示的密钥明文*/
    unsigned char keyBytes[8];/*8字节密钥明文*/
    
    if (keyLen != 32) {
        return 0;
    }
    /*首先对密钥密文进行解密, 得到16进制表示的密钥明文*/
    strKey = decrypt_str(sKey4Key, cipherKey, keyLen);
    if (strKey == 0)  {/*解密失败*/
        return 0;
    }
    
    /*然后对16进制表示的密钥明文编码成8字节的密钥密文*/
    hexStr2bytes(strKey, keyLen / 2, (char *)keyBytes);
    free(strKey);

    /*再使用8字节密钥明文对密码密文进行解密*/
    return decrypt_str(keyBytes, in, inLen);
    
}
