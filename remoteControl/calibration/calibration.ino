/*
 * Code to calibrate the ESC's for the soccer robots
 * 2000 = full forward
 * 1500 = neutral
 * 1000 = full reverse
 * WUSTL ASME Soccer Robots
 * Author: Patrick Naughton
 * 1/23/18
 */
#include <Servo.h>

int throttlePin = 5;
int reversePin = 6;
int escPin = 3;
Servo esc;

void setup(){
  Serial.begin(9600);
  pinMode(throttlePin, INPUT_PULLUP);
  pinMode(reversePin, INPUT_PULLUP);
  esc.attach(escPin);
}

void loop(){
  if(digitalRead(throttlePin) == LOW){
    Serial.println("high");
    esc.writeMicroseconds(2000);
  }else if(digitalRead(reversePin) == LOW){
    Serial.println("low");
    esc.writeMicroseconds(1000);
  }else{
    esc.writeMicroseconds(1500);
  }
}

