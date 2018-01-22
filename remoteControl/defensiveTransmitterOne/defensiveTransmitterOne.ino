/*
 * Author: Patrick Naughton
 * WUSTL ASME Soccer Robots
 * defensiveTransmitterOne
 * Desc: Use a wii nunchuck connected to an arduino Uno to control a robot.
 * Transmit this data with an RF24 module to a defense bot
 * Date: 1/21/2017
 */

#include <Wire.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>

const uint64_t PIPE = 0xE7E7E7E7E2LL;
#define NUM_INPUTS 4
#define PACKET_SIZE 6 //data that nunchuck sends back
#define ADDR 0x52
#define NUN_MAX 180 //subject to change, tentative mins and maxes of what we want to analogWrite?
#define NUN_MIN 0

//////////////////////////////PROTOTYPES//////////////////////////////
void nunchuck_init();
void nunchuck_send_request();
int nunchuck_get_data();
char nunchuck_decode_byte(char x);
byte nunchuck_Z(uint8_t * nunchuckBuffer);
byte nunchuck_C(uint8_t * nunchuckBuffer);
int nunchuck_X(uint8_t * nunchuckBuffer);
int nunchuck_Y(uint8_t * nunchuckBuffer);
static uint8_t *nunchuckBuffer = (uint8_t*)malloc(sizeof(uint8_t)*PACKET_SIZE); //make a buffer for the nunchuck

/*
 * Holds the inputs to the other arduino
 * msg[0] = right input
 * msg[1] = left input
 * msg[2] = shoot?
 */
int msg[NUM_INPUTS];

RF24 radio(9, 10);

void setup(){
  Serial.begin(250000);
  radio.begin();
  radio.openWritingPipe(PIPE);
  //radio.setAutoAck(false);
  //radio.setDataRate(RF24_2MBPS);
  radio.stopListening();
  nunchuck_init();
}

void loop(){
  //long start = millis();
  nunchuck_get_data(nunchuckBuffer);
  /*// Test code to get all inputs
  Serial.print("time to get_data = ");
  Serial.println(millis()-start);
  Serial.println();
  Serial.print("Z button = ");
  Serial.println(nunchuck_Z(nunchuckBuffer));
  Serial.print("C button = ");
  Serial.println(nunchuck_C(nunchuckBuffer));
  Serial.print("X = ");
  Serial.println(nunchuck_X(nunchuckBuffer));
  Serial.print("Y = ");
  Serial.println(nunchuck_Y(nunchuckBuffer));
  Serial.println();
  //*/
  if(NUM_INPUTS>0){
    //Serial.println("writing x");
    long sx = millis();
    msg[0] = nunchuck_X(nunchuckBuffer);
    radio.write(msg, sizeof(msg));
    //Serial.print("time for x = ");
    //Serial.println(millis()-sx);
  }
  if(NUM_INPUTS>1){
    //Serial.println("writing y");
    long sy = millis();
    msg[1] = nunchuck_Y(nunchuckBuffer);
    radio.write(msg, sizeof(msg));
    //Serial.print("time for y = ");
    //Serial.println(millis()-sy);
  }
  if(NUM_INPUTS>2){
    //Serial.println("writing z/c");
    long szc = millis();
    msg[2] = (nunchuck_Z(nunchuckBuffer)
    || nunchuck_C(nunchuckBuffer));
    radio.write(msg, sizeof(msg));
    //Serial.print("time for zc = ");
    //Serial.println(millis()-szc);
  }
  if(NUM_INPUTS>3){
    msg[3] = 1;
    radio.write(msg, sizeof(msg));
  }
//  long sr = millis();
//  radio.write(msg, sizeof(msg));
//  Serial.print("time for r = ");
//  Serial.println(millis()-sr);
//////////////////Testing what the msg is
//  Serial.print("msg[0] = ");
//  Serial.println(msg[0]);
//  Serial.print("msg[1] = ");
//  Serial.println(msg[1]);
//  Serial.print("msg[2] = ");
//  Serial.println(msg[2]);
//  Serial.print("msg[3] = ");
//  Serial.println(msg[3]);
/////////////////Testing timing with write at end
//  long startMsg = millis();
//  radio.write(msg, sizeof(msg));
//  Serial.println();
//  Serial.print("time to write msg = ");
//  Serial.println(millis()-start);
//  Serial.println();
}

void nunchuck_init(){
  Wire.begin();
  Wire.beginTransmission(ADDR);//opening handshake
  Wire.write(0x40);
  Wire.write(0x00);
  Wire.endTransmission();
}

void nunchuck_send_request(){
  Wire.beginTransmission(ADDR);//begin a transmission to nunchuck
  Wire.write(0x00);//write a zero
  Wire.endTransmission();
}

//fills in nunchuck_buf buffer with data
int nunchuck_get_data(uint8_t * nunchuck_buffer){
  int cnt = 0;
  Wire.requestFrom(ADDR, PACKET_SIZE);//request data from nunchuck
  //while(Wire.available()){
  for(int i = 0; i<PACKET_SIZE; i++){
    if(Wire.available()){
      *nunchuck_buffer = nunchuck_decode_byte(Wire.read());//put the next data in the buffer
      cnt++;//go to next pos in buffer
      nunchuck_buffer++;//increment nunchuck pointer
    }
  }
  nunchuck_send_request();//request next data
  if(cnt>=5){
    return 1;//if we got at least 6 bytes, success
  }else{
    return 0;//otherwise failure
  }
}

//decode the data that the nunchuck sends back
char nunchuck_decode_byte(char x){
  x = (x^0x17) + 0x17;
  return(x);
}

byte nunchuck_Z(uint8_t * nunchuckBuffer){
  return (((~(nunchuckBuffer[5]))>>0) & 1);
}

byte nunchuck_C(uint8_t * nunchuckBuffer){
  return (((~(nunchuckBuffer[5]))>>1) & 1);
}

int nunchuck_X(uint8_t * nunchuckBuffer){
  return map(nunchuckBuffer[0], 32, 226, NUN_MIN, NUN_MAX);
}

int nunchuck_Y(uint8_t * nunchuckBuffer){
  return map(nunchuckBuffer[1], 32, 226, NUN_MIN, NUN_MAX);
}


