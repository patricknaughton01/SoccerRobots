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
#define NUM_INPUTS 4
#define PACKET_SIZE 6 //data that nunchuck sends back
#define ADDR 0x52
#define NUN1 2 //the pin that switches to this line
#define NUN2 3 //the pin that switches to this line
#define NUN_MAX 127 //subject to change, tentative mins and maxes of what we want to analogWrite?
#define NUN_MIN -128

//////////////////////////////PROTOTYPES//////////////////////////////
void init_both_nunchucks();
void nunchuck_init();
void nunchuck_send_request();
int nunchuck_get_data();
char nunchuck_decode_byte(char x);
byte nunchuck_Z(uint8_t * nunchuckBuffer);
byte nunchuck_C(uint8_t * nunchuckBuffer);
int nunchuck_X(uint8_t * nunchuckBuffer);
int nunchuck_Y(uint8_t * nunchuckBuffer);

// 1 = right controller
// 2 = left controller
static uint8_t *nunchuckBuffer1 = (uint8_t*)malloc(sizeof(uint8_t)*PACKET_SIZE); //make a buffer for each nunchuck
static uint8_t *nunchuckBuffer2 = (uint8_t*)malloc(sizeof(uint8_t)*PACKET_SIZE);

/*
 * Holds the inputs to the other arduino
 * msg[0] = right input
 * msg[1] = left input
 * msg[2] = shoot?
 */
int msg[NUM_INPUTS];

RF24 radio(9, 10);

void setup(){
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(PIPE);
  nunchuck_init();
  pinMode(NUN1, OUTPUT);//initialize both nunchuck channels
  pinMode(NUN2, OUTPUT);//as closed
  digitalWrite(NUN1, LOW);
  digitalWrite(NUN2, LOW);
}

void loop(){
  nunchuck_get_data(nunchuckBuffer1);
  /*// Test code to get all inputs
  Serial.print("Z button = ");
  Serial.println(nunchuck_Z(nunchuckBuffer1));
  Serial.print("C button = ");
  Serial.println(nunchuck_C(nunchuckBuffer1));
  Serial.print("X = ");
  Serial.println(nunchuck_X(nunchuckBuffer1));
  Serial.print("Y = ");
  Serial.println(nunchuck_Y(nunchuckBuffer1));
  Serial.println();
  //*/
  if(NUM_INPUTS>0){
    Serial.println("writing y1");
    msg[0] = nunchuck_Y(nunchuckBuffer1);
  }
  if(NUM_INPUTS>1){
    Serial.println("writing y2");
    msg[1] = nunchuck_Y(nunchuckBuffer2);
  }
  if(NUM_INPUTS>2){
    Serial.println("writing z/c");
    msg[2] = (nunchuck_Z(nunchuckBuffer1)
    || nunchuck_C(nunchuckBuffer1)
    || nunchuck_Z(nunchuckBuffer2)
    || nunchuck_C(nunchuckBuffer2));
  }
  if(NUM_INPUTS>3){
    msg[3] = 1;
  }
  radio.write(msg, sizeof(msg));
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

