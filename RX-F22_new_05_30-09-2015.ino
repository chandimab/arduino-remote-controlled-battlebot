/*  DustyAlpha
 *  Receiver ver1.0
 *  Chandima B Samarasinghe ; 20 sept 2016
 *  
  PINS
  #Shield_Left
    pwm_L A0(14)
    pwm_R A1(15)
  #Shield_Right
    pwm_L A2(16)
    pwm_R A3(17)
  #Shield_Attack1 //OPTIONAL >>> Relay?
    pwm_L 3
    pwm_R 11
    rn_L 7
    rn_R 8
  #Attack2_Relay
    6
  #Servo_Arm
    pwm_arm 5
*/
#include <SPI.h> //to handle the communication interface with the modem
#include "nRF24L01.h" //to handle this particular modem driver
#include "RF24.h" //the library which helps us to control the radio modem
#include <Servo.h>

//MOTOR SHIELD CONNECTIONS
#define M_L_pwmL 5 
#define M_L_pwmR 6
#define M_L_enL 14 //A0
#define M_L_enR 15 //A1
  #define M_R_pwmL 18 //A4
  #define M_R_pwmR 19 //A5
  #define M_R_enL 16 //A2
  #define M_R_enR 17 //A3
//A6,A7 not working for output
#define attack1 3 //3 noise; check 10
#define attack2 9 //9 working fine

/* RF */
const uint64_t addresses[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
RF24 radio(7,8); // signals CE_PIN,CSE_PIN

int TXDATA[8] = {1,0,0,0,0,0,0,0}; //codex,L_pwmL,L_pwmR,R_pwmL,R_pwmR,Attack1,Attack2,Arm_pwm

/* controllers */
Servo serArm;
void setup_attack(){
  pinMode(attack1,OUTPUT);pinMode(attack2,OUTPUT);
}
void setup_motorShield(){
  pinMode(M_L_enL,OUTPUT);
  pinMode(M_L_enR,OUTPUT);
  pinMode(M_L_pwmL,OUTPUT);
  pinMode(M_L_pwmR,OUTPUT);
    pinMode(M_R_enL,OUTPUT);
    pinMode(M_R_enR,OUTPUT);
    pinMode(M_R_pwmL,OUTPUT);
    pinMode(M_R_pwmR,OUTPUT);
  digitalWrite(M_L_enL,1); digitalWrite(M_L_enR,1);
  digitalWrite(M_R_enL,1); digitalWrite(M_R_enR,1);
}
void ML(int pwmL,int pwmR){analogWrite(M_L_pwmL,pwmL);analogWrite(M_L_pwmR,pwmR);}
void MR(int pwmL,int pwmR){analogWrite(M_R_pwmL,pwmL);analogWrite(M_R_pwmR,pwmR);}
/*void Attack1(int a){if(a==1){digitalWrite(attack1,1);}else{digitalWrite(attack1,0);}}
void Attack2(int b){if(b==1){digitalWrite(attack2,1);}else{digitalWrite(attack2,0);}}
*/
void Attack1(int a){digitalWrite(attack1,a);}
void Attack2(int b){digitalWrite(attack1,b);}
void setup() {
  setup_motorShield(); setup_attack();
  Serial.begin(9600);
  Serial.println("DustyAlpha : Powered on");
  
  radio.begin(); //activate the modem
  radio.setDataRate(RF24_2MBPS);
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
  radio.openWritingPipe(addresses[0]); //sets the address of the receiver to which the program will send data.
  radio.openReadingPipe(1,addresses[1]); //channel(0-upto7), this device?
  
  radio.startListening(); // switch the modem to data receiving mode
  
  /* controllers*/
  serArm.attach(3);
  Serial.println("DustyAlpha : Ready2Destroy");
}

void loop() {
  //testRun();
  readRadio();
}

void readRadio(){
  if(radio.available()){
    radio.read(&TXDATA, sizeof(TXDATA));
    Serial.print(TXDATA[0]);Serial.print(" ");Serial.print(TXDATA[1]);Serial.print(" ");Serial.print(TXDATA[2]);Serial.print(" ");Serial.print(TXDATA[3]);Serial.print(" ");Serial.print(TXDATA[4]);Serial.print(" ");Serial.print(" ");Serial.print(TXDATA[5]);Serial.print(" ");Serial.print(" ");Serial.print(TXDATA[6]);Serial.print(" ");Serial.println(TXDATA[7]);

    if(TXDATA[0]=1993){ //noiseDetection CODE; geniune
      ML(TXDATA[1],TXDATA[2]);MR(TXDATA[3],TXDATA[4]);
     /* analogWrite(5,TXDATA[1]);analogWrite(6,TXDATA[2]);
      analogWrite(18,TXDATA[3]);analogWrite(19,TXDATA[4]);
      */
      Attack1(TXDATA[5]); Attack2(TXDATA[6]);
      //serArm.writeMicroseconds(TXDATA[7]);s
    }else{ //noiseAvailable
        //writing previous value to controllers
        //Serial.println("noise recieved");
      ML(0,0); MR(0,0);
      /*analogWrite(5,0);analogWrite(6,0);
      analogWrite(18,0);analogWrite(19,0);
      */
      Attack1(0); Attack2(0);
      //serArm.writeMicroseconds(TXDATA[7]);
    }  
    
  }else{
    //Serial.println("NO-SIGNAL");
     //writing default values to controllers
     ML(0,0); MR(0,0);
    /*analogWrite(5,0);analogWrite(6,0);
      analogWrite(18,0);analogWrite(19,0);
    */
    Attack1(0); Attack2(0);
      //serArm.writeMicroseconds(TXDATA[7]);
  }
  
}


void testRun(){
  ML(200,0);MR(200,0);
  delay(1000);
  ML(180,0);MR(0,0);
  delay(1000);
  ML(0,0);MR(0,0);
  delay(1000);
  ML(0,0);MR(180,0);
  delay(1000);
  ML(0,0);MR(0,0);
  delay(1000);
  ML(0,180);MR(0,180);
  delay(1000);
  ML(0,0);MR(0,0);
  delay(1000);
}
void testRun2(){
  analogWrite(5,200);analogWrite(6,0);
  analogWrite(18,200);analogWrite(19,0);
  delay(1000);
  analogWrite(5,0);analogWrite(6,0);
  analogWrite(18,0);analogWrite(19,0);
  delay(1000);

}

