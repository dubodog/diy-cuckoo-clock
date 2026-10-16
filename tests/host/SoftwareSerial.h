#pragma once
extern unsigned long tick,audioStart;extern int plays,servoPins[20];extern bool audioActive;
struct SoftwareSerial {SoftwareSerial(int,int){}void begin(int){}void write(const unsigned char* p,int n){if(p[3]==0x12){assert(servoPins[3]==45&&servoPins[5]==135&&servoPins[6]==30);plays++;audioStart=tick;audioActive=true;}if(p[3]==0x16)audioActive=false;}};
