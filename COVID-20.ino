/*
 * This code was written with the intention of using Arduino to control
 * a prototype elevator using Elechouse Voice Recognition Module.
 * 
 * To be seen by Mr. Ankur Sharma for Physics Honor Project
 * Not for public use. Thankyou.
 * This comment was written: 4/23/2020
 */


#include <SoftwareSerial.h> // Not used
#include "VoiceRecognitionV3.h" // Elechouse Voice-Recognition Module library
#include <CheapStepper.h> // Library used to control the 28BYJ-48 motor module

VR myVR(2,3); // Initialized at digital pins 2 and 3

uint8_t record[7]; // 7 Records per group. For more information, read the documentation.
uint8_t buf[64]; // array used by the library to store information regarding the recording. For more information, read the documentation.

int floorInt = 1; // the elevator will start at floor 1

CheapStepper stepper(9, 10, 11, 12); // initialize IN1 -> PIN9, IN2 -> PIN 10, IN3 -> PIN11, IN4 -> PIN12

int degPerFloor = 458.11; // calculated each floor to be exactly 458.11 degrees away from each other in terms of motor rotation

#define onRecord (0) // 1st Record
#define offRecord (1) // 2nd Record
#define thirdRecord (2) // 3rd Record

char printSignature (uint8_t *buf, int len) { // Function that returns each individual char of the signature name back to the loop() function
  for(int i = 0; i < len; i++) {
    
    if(buf[i] > 0x19 && buf[i] < 0x7F) {
      
      return(char(buf[i]));
    } else {
      Serial.print("[");
      Serial.print(buf[i]);
      Serial.print("]");
    }
  }
}

void printVR(uint8_t *buf) // Function used for testing, unimportant.
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.println(buf[2], DEC);
  Serial.println("\t\t");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3] >0) {
    printSignature(buf+4, buf[3]);
  }
}

void setup() { // Setup
  myVR.begin(9600); // Voice Recognizer receives information at 9600 baud
  stepper.setRpm(12); // Stepper motor has a maximum speed of 12
  Serial.begin(115200); // Baud rate needs to be set at 115200 to be able to receive information from the Voice Recognizer

  if(myVR.clear() == 0) { // Ensure that the Voice Recognition Module is still there
    Serial.println("Recognizer cleared.");
  } else {
    Serial.println("Not find VR Module");
    while(1);
  }

  if(myVR.load((uint8_t)onRecord) >= 0){ // load records for to be used
    Serial.println("onRecord loaded");
  }
  
  if(myVR.load((uint8_t)offRecord) >= 0){
    Serial.println("offRecord loaded");
  }
  if(myVR.load((uint8_t)thirdRecord) >= 0) {
    Serial.println("thirdRecord loaded");
  }
}

void loop() {
  int ret; // ret -> retainer
  ret = myVR.recognize(buf, 50); // recognize() returns a value when it recognizes the sound compared to the loaded recordings For more information, read the documentation
  if(ret > 0) { // constantly checking for identification of voice
    char receiveChar[3]; // initialize the char array that will take the char from the function printSignature()  
    
    for(int x = 0; x < buf[3]; x++) { // takes char from buf[4], buf[5], buf[6] and assings it to receiveChar[0, 1, 2]
     receiveChar[x] = (printSignature(buf+4+x, buf[3]));  
    }

    receiveChar[3] = '\0'; // assings the final position of char array to the null-terminated character
    
    Serial.println(receiveChar); // used for debugging, unimportant.

    if(receiveChar[0] == 'o' && receiveChar[1] == 'n' && receiveChar[2] == 'e') { // nested if-else-if statements to check the floor, and accordingly to the number received.
      if(floorInt == 1) {}

      if(floorInt == 2) {
        Serial.println("Running");
        stepper.moveDegreesCW(degPerFloor);
        floorInt = 1;
        Serial.println("Done!");
      }
      if(floorInt == 3) {
        Serial.println("Running");
        stepper.moveDegreesCW(degPerFloor * 2);
        floorInt = 1;
        Serial.println("Done!");
      }
    } else if(receiveChar[0] == 't' && receiveChar[1] == 'w' && receiveChar[2] == 'o') {
        if(floorInt == 1){
          Serial.println("Running");
          stepper.moveDegreesCCW(degPerFloor);
          floorInt = 2;
          Serial.println("Done!");
        }
        if(floorInt == 2){}

        if(floorInt == 3) {
          Serial.println("Running");
          stepper.moveDegreesCW(degPerFloor);
          floorInt = 2;
          Serial.println("Done!");
        }
      Serial.println("This is two");
    } else if(receiveChar[0] == 't' && receiveChar[1] == 'e' && receiveChar[2] == 'n') {
      Serial.println("This is ten");
        
        if(floorInt == 1){
          Serial.println("Running");
          stepper.moveDegreesCCW(degPerFloor*2);
          floorInt = 3;
          Serial.println("Done!");
        }
        if(floorInt == 3){}

        if(floorInt == 2) {
          Serial.println("Running");
          stepper.moveDegreesCCW(degPerFloor);
          floorInt = 3;
          Serial.println("Done!");
        }
    }
  }
}

// DONE!
