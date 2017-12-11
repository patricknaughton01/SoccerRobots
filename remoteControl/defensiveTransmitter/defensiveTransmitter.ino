/*
 * Author: Patrick Naughton
 * WUSTL ASME Soccer Robots
 * defensiveTransmitter
 * Desc: Use two wii nunchucks connected to an arduino Uno to control a robot.
 * Transmit this data with an RF24 module to a defense bot
 * Date: 12/10/2017
 */

#include <Wire.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>

#define PIPE 0xE7E7E7E7E2LL
#define NUM_INPUTS 3
#define PACKET_SIZE 6//data that nunchuck sends back
#define ADDR 0x52
#define NUN1 2//the pin that switches to this line
#define NUN2 3//the pin that switches to this line

void init_both_nunchucks();
void nunchuck_init();
void nunchuck_send_request();
int nunchuck_get_data();
char nunchuck_decode_byte(char x);

static uint8_t *nunchuckBuffer1 = (uint8_t*)malloc(sizeof(uint8_t)*PACKET_SIZE);
static uint8_t *nunchuckBuffer2 = (uint8_t*)malloc(sizeof(uint8_t)*PACKET_SIZE);

int msg[NUM_INPUTS];

RF24 radio(9, 10);

void setup(){
  radio.begin();
  radio.openWritingPipe(PIPE);
  //nunchuck_init();
  pinMode(NUN1, OUTPUT);//initialize both nunchuck channels
  pinMode(NUN2, OUTPUT);//as closed
  digitalWrite(NUN1, LOW);
  digitalWrite(NUN2, LOW);
}

void loop(){
  //nunchuck_get_data();
//  for(int i = 0; i<PACKET_SIZE; i++){
//    Serial.print("nunchuckBuffer1[");
//    Serial.print(i);
//    Serial.print("] = ");
//    Serial.println(nunchuckBuffer1[i]);
//  }
  Serial.println();
}

void init_both_nunchucks(){
  digitalWrite(NUN1, HIGH);//open NUN1 channel
  digitalWrite(NUN2, LOW);//and ensure NUN2 is closed
  nunchuck_init();//initialize nunchuck 1
  digitalWrite(NUN1, LOW);//close NUN1
  digitalWrite(NUN2, HIGH);//open NUN2
  nunchuck_init();//initialize nunchuck 2
  digitalWrite(NUN2, LOW);//close NUN2 to end
}

void get_both_nunchuck_data(){
  digitalWrite(NUN1, HIGH);//open NUN1 channel
  digitalWrite(NUN2, LOW);//and ensure NUN2 is closed
  nunchuck_get_data(nunchuckBuffer1);//read nunchuck 1's data into nunchuckBuffer1
  digitalWrite(NUN1, LOW);//close NUN1
  digitalWrite(NUN2, HIGH);//open NUN2
  nunchuck_get_data(nunchuckBuffer2);//read nunchuck 2's data into nunchuckBuffer2
  digitalWrite(NUN2, LOW);//close NUN2
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
  while(Wire.available()){
    *nunchuck_buffer = nunchuck_decode_byte(Wire.read());//put the next data in the buffer
    cnt++;//go to next pos in buffer
    nunchuck_buffer++;//increment nunchuck pointer
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

