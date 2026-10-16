#pragma once
#include <stdint.h>
extern bool sensorPresent,followTime;extern uint16_t fakeRaw[2];extern uint32_t rtcSeconds;extern unsigned long tick,rtcSetAt;
struct FakeWire {int address=0,channel=0,reg=0,count=0,index=0;unsigned char out[2];
 void begin(){}void setClock(long){}void setWireTimeout(long,bool){}
 void beginTransmission(int a){address=a;}void write(unsigned char b){if(address==0x70)channel=b==1?0:1;else reg=b;}
 int endTransmission(bool=true){return sensorPresent?0:1;}
 int requestFrom(unsigned char,unsigned char n){count=n;index=0;
  if(reg==0x0b)out[0]=0x20;
  else{unsigned int raw=fakeRaw[channel];if(followTime){unsigned long sec=rtcSeconds+(tick-rtcSetAt)/1000;float angle=channel==0?(sec%43200)/120.0f:(sec%3600)/10.0f;raw=(unsigned int)(angle*4096/360+.5f)%4096;}out[0]=raw>>8;out[1]=raw&255;}
  return n;}
 int read(){return out[index++];}
};extern FakeWire Wire;
