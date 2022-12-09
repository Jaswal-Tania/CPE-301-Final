#include <LiquidCrystal.h>
//initialize lcd screen 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Button/Transistor
volatile unsigned char* myPORT_K = (unsigned char*) 0x108;
volatile unsigned char* myDDR_K  = (unsigned char*) 0x107;
volatile unsigned char* myPIN_K  = (unsigned char*) 0x106;


//digital pin
volatile unsigned char* myPORT_B = (unsigned char*) 0x25; 
volatile unsigned char* myDDR_B  = (unsigned char*) 0x24;
volatile unsigned char* myPIN_B  = (unsigned char*) 0x23;

int val = 0;


void setup() {
  // put your setup code here, to run once:
  *myDDR_B |= 0x08;
  *myPORT_B &= 0x08;
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  int level = readSensor();

  Serial.print("Water level: ");
  Serial.println(level);

}

int readSensor() {
  *myPORT_B |= 0x08;  // Turn the sensor ON
  delay(10);              // wait 10 milliseconds
  val = analogRead(A15);    // Read the analog value form sensor
  *myPORT_B &= 0x08;   // Turn the sensor OFF
  return val;             // send current reading
}
