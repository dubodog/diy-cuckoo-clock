#pragma once
#include <stdint.h>
extern uint32_t rtcSeconds;extern unsigned long tick,rtcSetAt;extern bool rtcPresent,rtcLost;
struct DateTime {
 enum{TIMESTAMP_FULL};uint32_t sec=0;int yy=2026,mo=10,dd=1;
 DateTime(){}DateTime(uint32_t t):sec(t){}DateTime(int y,int m,int d,int h,int mi,int s):sec(h*3600+mi*60+s),yy(y),mo(m),dd(d){}
 int hour(){return sec/3600%24;}int minute(){return sec/60%60;}int second(){return sec%60;}
 int year(){return yy;}int month(){return mo;}int day(){return dd;}uint32_t unixtime(){return sec;}
 bool isValid(){return true;}const char* timestamp(int){return "fake";}
};
struct RTC_DS3231 {bool begin(){return rtcPresent;}bool lostPower(){return rtcLost;}DateTime now(){return DateTime(rtcSeconds+(tick-rtcSetAt)/1000);}void adjust(DateTime x){rtcSeconds=x.sec;rtcSetAt=tick;rtcLost=false;}};
