/*
 * Author: Patrick Naughton
 * WUSTL ASME Soccer Robots
 * defensive Receiver
 * Desc: Use the RF24 radio module to interpret signals coming from the transmitter.
 * Move the robot based on these signals.
 * Date: 1/18/2017
 */

#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <Servo.h>

const uint64_t PIPE = 0xE7E7E7E7E2LL; //frequency this receives on
#define NUM_INPUTS 5 //number of devices we have to independently control on the robot plus a validation signal

int msg[NUM_INPUTS];//place to store data from radio
int motorPins[NUM_INPUTS];//hold pin numbers of motors (really escs)

Servo escs[NUM_INPUTS];

RF24 radio(9, 10);

void setup(){
  Serial.begin(9600);
  ///////////////Hard code three motors///////////////
  motorPins[0] = 3;
  motorPins[1] = 5;
  motorPins[2] = 6;
  escs[0].attach(motorPins[0]);
  escs[1].attach(motorPins[1]);
  escs[2].attach(motorPins[2]);
  for(int i = 0; i<3; i++){
    escs[i].writeMicroseconds(1500);
  }
  ///////////////End hard code///////////////
  escs[0].writeMicroseconds(2000);
  delay(2000);
  escs[0].writeMicroseconds(1000);
  delay(3000);
  escs[0].writeMicroseconds(1500);
  radio.begin();
  radio.openReadingPipe(1, PIPE);
  radio.startListening();
  Serial.begin(9600);
}

void loop(){
  if(radio.available()){
    radio.read(msg, sizeof(int)*NUM_INPUTS);
  }
//  else{
//    msg[0] = 1500;
//    msg[1] = 1500;
//    msg[2] = 1500;
//    msg[3] = 1500;
//    msg[4] = 0;
//  }
  /*for(int i = 0; i<NUM_INPUTS; i++){
    Serial.print(msg[i]);
    Serial.print(", ");
  }*/
  Serial.println();
  if(msg[4]!=0){
    escs[0].writeMicroseconds(msg[2]);         // shooter
    int rightMotorRawVal = 1500 + msg[1] - msg[0];
    int rightMotor = map(rightMotorRawVal, 1000, 2000, 1000,  2000);
    int leftMotorRawVal = rightMotorRawVal; //3000 - (msg[1] + msg[0] - 1500);
    int leftMotor = map(leftMotorRawVal, 1000, 2000, 1000, 2000);
    escs[1].writeMicroseconds(rightMotor);    // right motor
    escs[2].writeMicroseconds(leftMotor);     // left motor
  }else{
    escs[0].writeMicroseconds(1500);
    escs[1].writeMicroseconds(1500);
    escs[2].writeMicroseconds(1500);
  }
  delay(10);
}

