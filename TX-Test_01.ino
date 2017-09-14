/* Transmitter Testing ver1.0
Chandima B Samarasinghe
*/
#include <SPI.h> //to handle the communication interface with the modem
#include "nRF24L01.h" //to handle this particular modem driver
#include "RF24.h" //the library which helps us to control the radio modem


const uint64_t addresses[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
RF24 radio(9,10); // signals CE_PIN 9,CSE_PIN10


void setup() {
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
}

int msg[]={1,2222};
void loop() {
  Serial.println(msg[1]);
  radio.write(&msg,sizeof(msg));
}

