#include <initializer_list>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <stdint.h>
using byte=unsigned char;using __FlashStringHelper=char;
#define F(x) x
#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define A0 14
#define A1 15
unsigned long tick=0,rtcSetAt=0,audioStart=0;uint32_t rtcSeconds=0;
bool rtcPresent=true,rtcLost=false,sensorPresent=true,followTime=true,setDown=false,isPM=false,audioActive=false,audioFail=false;
uint16_t fakeRaw[2]={0,0};int servoPins[20]={},pins[20]={},plays=0;
unsigned long millis(){return tick;}void delay(unsigned long d){tick+=d;}void pinMode(int,int){}void digitalWrite(int p,int v){pins[p]=v;}
int digitalRead(int p){if(p==12)return setDown?LOW:HIGH;if(p==15)return isPM?LOW:HIGH;if(p==7)return !audioFail&&audioActive&&tick-audioStart>=30&&tick-audioStart<1080?LOW:HIGH;return HIGH;}
struct FakeSerial {void begin(int){}template<class T>void println(T){}template<class T>void print(T){}int available(){return 0;}char read(){return 0;}} Serial;
#include "Wire.h"
#include "RTClib.h"
#include "EEPROM.h"
FakeWire Wire;FakeEEPROM EEPROM;

#include "../../firmware/cuckoo_clock/cuckoo_clock.ino"

void advance(unsigned long duration){unsigned long until=tick+duration;while(tick<until){loop();
 assert(abs((LEFT_CLOSED-leftAngle)-(rightAngle-RIGHT_CLOSED))<=1);
 if(stage==DOORS_OPENING){assert(leftTarget==LEFT_OPEN&&rightTarget==RIGHT_OPEN);assert(birdAngle==BIRD_HOME);}
 if(stage==DOORS_CLOSING){assert(leftTarget==LEFT_CLOSED&&rightTarget==RIGHT_CLOSED);assert(birdAngle==BIRD_HOME);}
 if(birdAngle!=BIRD_HOME){assert(leftAngle==LEFT_OPEN&&rightAngle==RIGHT_OPEN);}
 }}
void ready(int hour,int minute=0,int second=0){
 rtc.adjust(DateTime(2026,10,1,hour,minute,second));rtcOK=timeOK=true;faulted=false;stage=IDLE;
 leftAngle=LEFT_CLOSED;rightAngle=RIGHT_CLOSED;birdAngle=BIRD_HOME;leftTarget=leftAngle;rightTarget=rightAngle;birdTarget=birdAngle;
 leftDoor.write(leftAngle);rightDoor.write(rightAngle);bird.write(birdAngle);lastChimedMinute=0xffffffffUL;
 audioFail=false;audioActive=false;plays=0;
}
int main(){
 for(int hh=0;hh<24;hh++){assert(quietHour(hh)==(hh<8||hh>=21));assert(hourCount(hh)==(hh%12?hh%12:12));assert(hourlyDue(hh,0,0,1,0)==(hh>=8&&hh<21));assert(!hourlyDue(hh,0,0,1,1));assert(!hourlyDue(hh,1,0,1,0));assert(!hourlyDue(hh,0,3,1,0));}
 byte p[10],want[]={0x7e,0xff,6,0x12,0,0,1,0xfe,0xe8,0xef};playerPacket(0x12,1,p);assert(!memcmp(p,want,10));
 rtcLost=true;setup();assert(!timeOK);advance(1000);assert(!plays&&stage==IDLE);
 for(int hh=8;hh<=20;hh++){ready(hh);advance(40000);assert(plays==hourCount(hh));assert(stage==IDLE);assert(leftAngle==LEFT_CLOSED&&rightAngle==RIGHT_CLOSED&&birdAngle==BIRD_HOME);int once=plays;advance(1000);assert(plays==once);}
 for(int hh: {0,7,21,23}){ready(hh);char test[]="T";handleCommand(test);advance(6000);assert(plays==0&&stage==IDLE);}
 ready(8,30);char set[]="S 2026 10 1 15 0 0";handleCommand(set);advance(40000);assert(plays==0&&rtc.now().hour()==15);
 ready(7,59,59);advance(41000);assert(plays==8&&stage==IDLE);
 ready(20,59,59);advance(5000);assert(!plays&&stage==IDLE);
 ready(10);audioFail=true;advance(25000);assert(faulted&&stage==IDLE&&plays==1);assert(birdAngle==BIRD_HOME&&rightAngle==RIGHT_CLOSED&&leftAngle==LEFT_CLOSED);
 ready(9,30);startChime(2);advance(500);char cancelOpening[]="C";handleCommand(cancelOpening);advance(10000);assert(stage==IDLE&&leftAngle==LEFT_CLOSED&&rightAngle==RIGHT_CLOSED);
 ready(9);advance(7000);char close[]="C";handleCommand(close);advance(10000);assert(stage==IDLE&&birdAngle==BIRD_HOME&&leftAngle==LEFT_CLOSED&&rightAngle==RIGHT_CLOSED);
 puts("PASS V3: 24-hour rules;13 complete cycles;exact counts;quiet manual test;no immediate chime after S;08 recovery;21 silence;audio failure closes;manual close;cancel during opening;simultaneous door targets and motion;bird-door interlock;UART checksum");
}
