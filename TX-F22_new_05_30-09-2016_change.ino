/*  DustyAlpha
 *  Transmitter ver1.0
 *  Chandima B Samarasinghe ; 20 sept 2016
*/
#include <SPI.h> //to handle the communication interface with the modem
#include "nRF24L01.h" //to handle this particular modem driver
#include "RF24.h" //the library which helps us to control the radio modem

/* inputs */
#define JOYSTICK_R_X 16 
#define JOYSTICK_R_Y 17 
#define JOYSTICK_R_K 4

#define JOYSTICK_L_X 14 
#define JOYSTICK_L_Y 15
#define JOYSITCK_L_K 3

#define var_left_top 18
#define var_left_front 19
#define var_right_top 20
#define var_right_front 21

#define twoway_left_1 7
#define twoway_left_2 8
#define twoway_right_1 12
#define twoway_right_2 13

#define led 5

#define codex 1993 //noiseDetectionCode

int neutJoyRX,neutJoyRY,neutJoyLX,neutJoyLY;
int valJoyRX,valJoyRY,valJoyLX,valJoyLY; //to hold joystick values

//Movement variables
int s=0;
int f=0;
int sensitivity=5;
const uint64_t addresses[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
RF24 radio(9,10); // signals CE_PIN 9,CSE_PIN10

int TXDATA[8] = {codex,0,0,0,0,0,0,0}; //codex,L_pwmL,L_pwmR,R_pwmL,R_pwmR,Attack1,Attack2,Arm_pwm

void pinSetup(){
  pinMode(JOYSTICK_R_X,INPUT);
  pinMode(JOYSTICK_R_Y,INPUT);
  pinMode(JOYSTICK_R_K,INPUT);
    pinMode(JOYSTICK_L_X,INPUT);
    pinMode(JOYSTICK_L_Y,INPUT);
    pinMode(JOYSITCK_L_K,INPUT);
  pinMode(var_left_top,INPUT);
  pinMode(var_left_front,INPUT);
  pinMode(var_right_top,INPUT);
  pinMode(var_right_front,INPUT);
    pinMode(twoway_left_1,INPUT);
    pinMode(twoway_left_2,INPUT);
    pinMode(twoway_right_1,INPUT);
    pinMode(twoway_right_2,INPUT);
  pinMode(led,OUTPUT);
}
void setup() {

  pinSetup();
  Serial.begin(9600);
  Serial.println("Transmitter Powered On");
  
  radio.begin(); //activate the modem
  radio.setDataRate(RF24_2MBPS);
  radio.setRetries(0,0);
  
  /*
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
                                            It shows how many times the modem will retry to the send data in case of not receiving by another modem.
                                            The first argument sets how often modem will retry. It is a multiple of 250 microseconds. 15 * 250 = 3750. So, if the recipient does not receive data, 
                                            modem will try to send them every 3.75 milliseconds. Second argument is the number of attempts. So in our example,modem will try to send 15 times before
                                            it will stop and finds that the receiver is out of range, or is turned off.
  radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  
  radio.startListening(); //OR stopListening
  */
  radio.openWritingPipe(addresses[1]); //sets the address of the receiver to which the program will send data.
  radio.openReadingPipe(1,addresses[0]); //channel(0-upto7), this device?
  
  radio.stopListening(); // switch the modem to data transmission mode
  
  /*getting neutral positions */
  Serial.println("Neutral-Mode Calibration : Start");
    calibInitial();
  Serial.println("Neutral-Mode Calibration : Complete");
}

void loop() {
    //getting row values
  valJoyRX=1023-analogRead(JOYSTICK_R_X);
  valJoyRY=1023-analogRead(JOYSTICK_R_Y);
  valJoyLX=1023-analogRead(JOYSTICK_L_X);
  valJoyLY=analogRead(JOYSTICK_L_Y);
/*  Serial.print(valJoyLY);
    Serial.print(" ");
  Serial.print(valJoyLX);
    Serial.print(" ");
  Serial.print(valJoyRY);
    Serial.print(" ");
  Serial.println(valJoyRX); */

  // 0      1      2      3      4
  //codex,L_pwmL,L_pwmR,R_pwmL,R_pwmR,Attack1,Attack2,Arm_pwm
  if(valJoyLY>neutJoyLY+sensitivity){
    f=map(valJoyLY,neutJoyLY+sensitivity,1023,0,255);
    if(valJoyRX>neutJoyRX){
      s=map(valJoyRX,neutJoyRX,1023,0,255);
      TXDATA[1]=min(f+s,255); TXDATA[3]=f;
      TXDATA[2]=0;TXDATA[4]=0;
    }else if(valJoyRX<neutJoyRX){
      s=map(valJoyRX,0,neutJoyRX,255,0);
      TXDATA[1]=f; TXDATA[3]=min(f+s,255);
      TXDATA[2]=0;TXDATA[4]=0;
    }else{
      TXDATA[1]=f; TXDATA[3]=f;
      TXDATA[2]=0;TXDATA[4]=0;
    }
  }else if(valJoyLY<neutJoyLY-sensitivity){
    f=map(valJoyLY,0,neutJoyLY-sensitivity,255,0);
    if(valJoyRX>neutJoyRX){
      s=map(valJoyRX,neutJoyRX,1023,0,255);
      TXDATA[1]=0; TXDATA[3]=0;
      TXDATA[2]=min(f+s,255);TXDATA[4]=f;
    }else if(valJoyRX<neutJoyRX){
      s=map(valJoyRX,0,neutJoyRX,255,0);
      TXDATA[1]=0; TXDATA[3]=0;
      TXDATA[2]=f;TXDATA[4]=min(f+s,255);
    }else{
      TXDATA[1]=0; TXDATA[3]=0;
      TXDATA[2]=f;TXDATA[4]=f;
    }
  }else{
    if(valJoyRX>neutJoyRX){
      s=map(valJoyRX,neutJoyRX,1023,0,255);
      TXDATA[1]=s; TXDATA[3]=0;
      TXDATA[2]=0;TXDATA[4]=s;
    }else if(valJoyRX<neutJoyRX){
      s=map(valJoyRX,0,neutJoyRX,255,0);
      TXDATA[1]=0; TXDATA[3]=s;
      TXDATA[2]=s;TXDATA[4]=0;
    }else{
      TXDATA[1]=0; TXDATA[3]=0;
      TXDATA[2]=0;TXDATA[4]=0;
    }
  }
 // if(digitalRead(twoway_left_1)==1){TXDATA[6]=1;}else{TXDATA[6]=0;}
  //if(digitalRead(twoway_right_1)==1){TXDATA[5]=1;}else{TXDATA[5]=0;}
  
 /*
  Serial.print(TXDATA[5]);
    Serial.print(" ");
  Serial.print(TXDATA[6]);
    Serial.print(" ");
  Serial.print(digitalRead(twoway_right_1));
    Serial.print(" ");
  Serial.println(digitalRead(twoway_right_2));  */

/*
  Serial.print(valJoyLY);
    Serial.print(" ");
  Serial.println(valJoyRX);
  */

  
  radio.write(&TXDATA,sizeof(TXDATA));
  //Serial.println(TXDATA[3]);

}


/* custom functions */
void calibInitial(){
  neutJoyRX=1023-analogRead(JOYSTICK_R_X);
  neutJoyRY=1023-analogRead(JOYSTICK_R_Y);;
  neutJoyLX=1023-analogRead(JOYSTICK_L_X);
  neutJoyLY=analogRead(JOYSTICK_L_Y);
}

