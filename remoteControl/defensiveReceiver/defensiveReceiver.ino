/*
 * Author: Patrick Naughton
 * WUSTL ASME Soccer Robots
 * defensive Receiver
 * Desc: Use the RF24 radio module to interpret signals coming from the transmitter.
 * Move the robot based on these signals.
 * Date: 12/10/2017
 */

#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <Servo.h>

const uint64_t PIPE = 0xE7E7E7E7E2LL; //frequency this receives on
#define NUM_INPUTS 4 //number of devices we have to independently control on the robot

int msg[NUM_INPUTS];//place to store data from radio
int motorPins[NUM_INPUTS];//hold pin numbers of motors (really escs)

Servo escs[NUM_INPUTS];

RF24 radio(9, 10);

void setup(){
  Serial.begin(9600);
  ///////////////Hard code three motors///////////////
  motorPins[0] = 9;
  motorPins[1] = 10;
  motorPins[2] = 11;
  escs[0].attach(motorPins[0]);
  escs[1].attach(motorPins[1]);
  escs[2].attach(motorPins[2]);
  ///////////////End hard code///////////////
  for(int i = 0; i<NUM_INPUTS; i++){
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }

  radio.begin();
  radio.openReadingPipe(1, PIPE);
  radio.startListening();
  
}

void loop(){
  if(radio.available()){
    radio.read(msg, sizeof(int)*NUM_INPUTS);
  }
  for(int i = 0; i<NUM_INPUTS-1; i++){
    Serial.print(msg[0]);
    Serial.print(",");
    Serial.print(msg[1]);
    Serial.print(",");
    Serial.print(msg[2]);
    Serial.print("\n");
    //escs[i].write(msg[i]);
  }
  delay(100);
}

