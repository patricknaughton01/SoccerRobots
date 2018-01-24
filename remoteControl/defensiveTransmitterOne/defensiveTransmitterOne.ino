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
#define NUM_INPUTS 3
#define PACKET_SIZE 6 //data that nunchuck sends back
#define ADDR 0x52
#define NUN_MAX 2000 //subject to change, tentative mins and maxes of what we want to analogWrite?
#define NUN_MIN 1000

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
 * msg[0] = x input
 * msg[1] = y input
 * msg[2] = z pressed?
 * msg[3] = c pressed?
 */
int msg[NUM_INPUTS];

RF24 radio(9, 10);

void setup(){
  radio.begin();
  radio.openWritingPipe(PIPE);
  radio.stopListening();
  nunchuck_init();
}

void loop(){
  nunchuck_get_data(nunchuckBuffer);
  if(NUM_INPUTS>0){
    msg[0] = nunchuck_X(nunchuckBuffer);
    radio.write(msg, sizeof(msg));
  }
  if(NUM_INPUTS>1){
    msg[1] = nunchuck_Y(nunchuckBuffer);
    radio.write(msg, sizeof(msg));
  }
  if(NUM_INPUTS>2){
    msg[2] = nunchuck_Z(nunchuckBuffer);
    radio.write(msg, sizeof(msg));
  }
  if(NUM_INPUTS>3){
    msg[3] = nunchuck_C(nunchuckBuffer);
    radio.write(msg, sizeof(msg));
  }
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


