#pragma once
#include <cstring>
struct FakeEEPROM {unsigned char bytes[64]={};template<class T>void get(int a,T& t){memcpy(&t,bytes+a,sizeof(T));}template<class T>void put(int a,const T& t){memcpy(bytes+a,&t,sizeof(T));}};extern FakeEEPROM EEPROM;
