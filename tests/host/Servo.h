#pragma once
extern int servoPins[20]; struct Servo {int pin;void attach(int p){pin=p;}void write(int a){servoPins[pin]=a;}};
