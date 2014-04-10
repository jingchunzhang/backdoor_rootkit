#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
//根据sock fd取对端ip地址
extern uint32_t getpeerip(int fd);
//根据网络设备名取ip地址
extern uint32_t getipbyif(const char* ifname);
void getinfo();
extern char hostinfo[256]; 
#endif
