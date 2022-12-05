// Project members: Tania Jaswal and Jasmine Wells
// File name: swampcooler.o

//included libraries

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <dht11.h>
#include <RTClib.h>


//initialize lcd screen 
LiquidCrystal lcd(7,8,9,10,11,12);


// Port Registers

// LEDs (Output)
volatile unsigned char* myPORTB = (unsigned char*) 0x25; 
volatile unsigned char* myDDRB  = (unsigned char*) 0x24;
volatile unsigned char* myPINB  = (unsigned char*) 0x23;

// Temperature and Humdity (Input)
volatile unsigned char* myPORT_C = (unsigned char*) 0x28;
volatile unsigned char* myDDR_C  = (unsigned char*) 0x27;
volatile unsigned char* myPIN_C  = (unsigned char*) 0x26;

// Fan Motor
volatile unsigned char* myPORTE = (unsigned char*) 0x2E; 
volatile unsigned char* myDDRE  = (unsigned char*) 0x2D;
volatile unsigned char* myPINE  = (unsigned char*) 0x2C;

// Water Level (Input)
volatile unsigned char* myPORT_F = (unsigned char*) 0x31;
volatile unsigned char* myDDR_F  = (unsigned char*) 0x30;
volatile unsigned char* myPIN_F  = (unsigned char*) 0x2F;

// Push Button(Input)
volatile unsigned char* myPORT_H = (unsigned char*) 0x102;
volatile unsigned char* myDDR_H  = (unsigned char*) 0x101;
volatile unsigned char* myPIN_H  = (unsigned char*) 0x100;

// Button/Transistor
volatile unsigned char* myPORT_K = (unsigned char*) 0x108;
volatile unsigned char* myDDR_K  = (unsigned char*) 0x107;
volatile unsigned char* myPIN_K  = (unsigned char*) 0x106;

// Timer Registers
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIFR1  = (unsigned char *) 0x36;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;

// ADC Registers 
volatile unsigned char* myADCSRA = (unsigned char*) 0x7A;
volatile unsigned char* myADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* myADMUX  = (unsigned char*) 0x7C;
volatile unsigned int* myADCDATA = (unsigned int*)  0x78;

// Pin change Interrupt Registers
volatile unsigned char* myPCMSK1 = (unsigned char *) 0x6C;
volatile unsigned char* myPCICR  = (unsigned char *) 0x68;


dht11 DHT11;

float temperature = 0;
float humidity = 0;

unsigned int state_count = 0;


// Functions

void idle_state(int water_level, float temperature1);

void error_state(int water_level, float temperature1;

void running_state(int water_level, float temperature1);

float lcd_display (float temperature1, float humidity);

void setup() {

    Serial.begin(9600);

}

void loop() {

    // LCD Display

    lcd.clear();
    lcd.print("Temp: ");        
    lcd.print(temperature);     // Displays Temperature Value 
    lcd.print("C");             // prints "C" for Celsius

    lcd.print("Humidity: ");
    lcd.print(humidity);        // Displays Humidity Value 
    lcd.print("%");


    // Serial Monitor Display

//     Serial.print("Water Level: ");
//     Serial.print(water_level);
//     Serial.print("      Temperature: ");
//     Serial.print(temperature);
//     Serial.print("C      Humidity: ");
//     Serial.print(humidity);
//     Serial.print("%");

    // Call Functions

}



// Define Functions

void idle_state(int water_level, float temperature1){}

void error_state(int water_level, float temperature1){}

void running_state(int water_level, float temperature1){}

float lcd_display (float temperature1, float humidity){}
