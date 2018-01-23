#include <Servo.h>

int throttlePin = 5;
int reversePin = 6;
int escPin = 3;
Servo esc;

void setup(){
  pinMode(throttlePin, INPUT_PULLUP);
  pinMode(reversePin, INPUT_PULLUP);
  esc.attach(escPin);
}

void loop(){
  if(digitalRead(throttlePin) == LOW){
    esc.write(180);
  }else if(digitalRead(reversePin) == LOW){
    esc.write(0);
  }else{
    esc.write(90);
  }
}

