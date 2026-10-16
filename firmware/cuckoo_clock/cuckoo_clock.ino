#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <ctype.h>
#include "Logic.h"
#include "Config.h"
const byte LEFT_PIN=3,RIGHT_PIN=5,BIRD_PIN=6,BUSY_PIN=7,LED_PIN=13;
RTC_DS3231 rtc;
SoftwareSerial player(10,11);
Servo leftDoor,rightDoor,bird;
bool rtcOK=false,timeOK=false,faulted=false;
unsigned long lastServo=0;
int leftAngle=LEFT_CLOSED,rightAngle=RIGHT_CLOSED,birdAngle=BIRD_HOME;
int leftTarget=LEFT_CLOSED,rightTarget=RIGHT_CLOSED,birdTarget=BIRD_HOME;
uint32_t lastChimedMinute=0xffffffffUL;
char command[64];byte commandLength=0;
enum Stage {IDLE,DOORS_OPENING,BIRD_EXTENDING,SETTLE,
 AUDIO_START,AUDIO_WAIT_START,AUDIO_WAIT_END,GAP,BIRD_RETURNING,DOORS_CLOSING};
Stage stage=IDLE;
unsigned long stageSince=0;
byte requestedCalls=0,completedCalls=0;
void audio(byte kind,uint16_t value=0) {byte p[10];playerPacket(kind,value,p);player.write(p,10);}
void changeStage(Stage next) {stage=next;stageSince=millis();}
void closeSafely() {
 if(stage!=IDLE)audio(0x16);
 birdTarget=BIRD_HOME;
 changeStage(BIRD_RETURNING); // Retract the bird before closing both doors together.
}

void fail(const __FlashStringHelper* why) {
 if(!faulted)Serial.println(why);
 faulted=true;
 if(stage!=IDLE&&stage!=BIRD_RETURNING&&stage!=DOORS_CLOSING)closeSafely();
}
bool runAllowed(){return rtcOK&&timeOK&&!faulted;}
bool soundAllowed(){return runAllowed()&&!quietHour(rtc.now().hour());}
void servoStep(Servo& servo,int& angle,int target) {
 if(angle==target)return;angle+=(target>angle?1:-1);servo.write(angle);
}
void servoService() {
 if(millis()-lastServo<20)return;lastServo=millis();
 servoStep(leftDoor,leftAngle,leftTarget);servoStep(rightDoor,rightAngle,rightTarget);servoStep(bird,birdAngle,birdTarget);
}
void chimeService() {
 if(stage==IDLE)return;
 if(stage!=BIRD_RETURNING&&stage!=DOORS_CLOSING&&!soundAllowed()){closeSafely();return;}
 switch(stage){
 case DOORS_OPENING:if(leftAngle==LEFT_OPEN&&rightAngle==RIGHT_OPEN){birdTarget=BIRD_OUT;changeStage(BIRD_EXTENDING);}break;
 case BIRD_EXTENDING:if(birdAngle==BIRD_OUT)changeStage(SETTLE);break;
 case SETTLE:if(millis()-stageSince>=300)changeStage(AUDIO_START);break;
 case AUDIO_START:
  if(digitalRead(BUSY_PIN)==LOW){audio(0x16);fail(F("AUDIO ERROR: already busy"));break;}
  audio(0x12,1);changeStage(AUDIO_WAIT_START);break;
 case AUDIO_WAIT_START:
  if(digitalRead(BUSY_PIN)==LOW)changeStage(AUDIO_WAIT_END);
  else if(millis()-stageSince>=2000){audio(0x16);fail(F("AUDIO ERROR: missing track or BUSY"));}
  break;
 case AUDIO_WAIT_END:
  if(digitalRead(BUSY_PIN)==HIGH){completedCalls++;changeStage(GAP);}
  else if(millis()-stageSince>=5000){audio(0x16);fail(F("AUDIO ERROR: playback timeout"));}
  break;
 case GAP:
  if(millis()-stageSince>=350){if(completedCalls<requestedCalls)changeStage(AUDIO_START);else{birdTarget=BIRD_HOME;changeStage(BIRD_RETURNING);}}
  break;
 case BIRD_RETURNING:if(birdAngle==BIRD_HOME){leftTarget=LEFT_CLOSED;rightTarget=RIGHT_CLOSED;changeStage(DOORS_CLOSING);}break;
 case DOORS_CLOSING:if(leftAngle==LEFT_CLOSED&&rightAngle==RIGHT_CLOSED)changeStage(IDLE);break;
 default:break;
 }
}
void startChime(byte n) {
 if(!soundAllowed()||stage!=IDLE)return;
 requestedCalls=n;completedCalls=0;leftTarget=LEFT_OPEN;rightTarget=RIGHT_OPEN;changeStage(DOORS_OPENING);
}

void printStatus(){
 if(rtcOK&&timeOK)Serial.println(rtc.now().timestamp(DateTime::TIMESTAMP_FULL));
 Serial.print(F("rtc/time/fault "));Serial.print(rtcOK);Serial.print(' ');Serial.print(timeOK);Serial.print(' ');Serial.println(faulted);
}
void handleCommand(char* input){
 char kind=toupper(input[0]);
 if(kind=='P')printStatus();
 else if(kind=='T'){if(soundAllowed())startChime(2);else Serial.println(F("TEST SILENT: time missing, fault or quiet hours"));}
 else if(kind=='C')closeSafely();
 else if(kind=='S'){
  int y,mo,d,h,mi,s;
  if(rtcOK&&sscanf(input+1,"%d %d %d %d %d %d",&y,&mo,&d,&h,&mi,&s)==6&&y>=2000&&y<=2099&&mo>=1&&mo<=12&&d>=1&&d<=31&&h>=0&&h<=23&&mi>=0&&mi<=59&&s>=0&&s<=59){
   DateTime proposed(y,mo,d,h,mi,s);
   if(proposed.isValid()){
    if(stage!=IDLE)closeSafely();
    rtc.adjust(proposed);timeOK=true;faulted=false;lastChimedMinute=rtc.now().unixtime()/60UL;
    Serial.println(F("RTC saved. Set quartz clock separately with rear knob. No immediate chime."));return;
   }
  }Serial.println(F("Invalid. Example: S 2026 10 1 8 0 0"));
 }
 else Serial.println(F("P=status T=2-call test C=close S yyyy mo dd hh mi ss"));
}
void serialService() {
 while(Serial.available()){
  char c=Serial.read();if(c=='\r')continue;
  if(c=='\n'){command[commandLength]=0;if(commandLength)handleCommand(command);commandLength=0;}
  else if(commandLength<sizeof(command)-1)command[commandLength++]=c;else commandLength=0;
 }
}

void setup(){
 Serial.begin(9600);pinMode(BUSY_PIN,INPUT_PULLUP);pinMode(LED_PIN,OUTPUT);
 leftDoor.attach(LEFT_PIN);rightDoor.attach(RIGHT_PIN);bird.attach(BIRD_PIN);
 leftDoor.write(LEFT_CLOSED);rightDoor.write(RIGHT_CLOSED);bird.write(BIRD_HOME);
 Wire.begin();Wire.setClock(100000);Wire.setWireTimeout(25000,true);
 rtcOK=rtc.begin();timeOK=rtcOK&&!rtc.lostPower()&&rtc.now().isValid();
 player.begin(9600);delay(4000);audio(0x16);delay(30);audio(0x09,2);delay(200);audio(0x19,1);delay(30);audio(0x06,PLAYER_VOLUME);
 if(rtcOK&&timeOK)lastChimedMinute=rtc.now().unixtime()/60UL;
 Serial.println(F("V3: quartz hands and RTC are separate. Use S to set chime time; set hands with rear knob."));
}
void loop(){
 serialService();servoService();chimeService();
 if(runAllowed()&&stage==IDLE){
  DateTime now=rtc.now();
  if(!now.isValid()){timeOK=false;fail(F("RTC read invalid: check wiring"));}
  else{uint32_t id=now.unixtime()/60UL;
   if(hourlyDue(now.hour(),now.minute(),now.second(),id,lastChimedMinute)){
    lastChimedMinute=id;startChime(hourCount(now.hour()));
   }
  }
 }
 digitalWrite(LED_PIN,(!rtcOK||!timeOK||faulted)?((millis()/350)%2):LOW);
 delay(2);
}
