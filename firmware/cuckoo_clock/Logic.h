#pragma once
#include <stdint.h>
inline bool quietHour(uint8_t h) {return h>=21||h<8;}
inline uint8_t hourCount(uint8_t h) {uint8_t n=h%12;return n?n:12;}
inline bool hourlyDue(uint8_t h,uint8_t m,uint8_t s,uint32_t id,uint32_t last) {return !quietHour(h)&&m==0&&s<3&&id!=last;}
inline void playerPacket(uint8_t kind,uint16_t value,uint8_t *p) {
 p[0]=0x7e;p[1]=0xff;p[2]=6;p[3]=kind;p[4]=0;p[5]=value>>8;p[6]=value&255;
 uint16_t sum=0;for(uint8_t i=1;i<=6;i++)sum-=p[i];p[7]=sum>>8;p[8]=sum&255;p[9]=0xef;
}
