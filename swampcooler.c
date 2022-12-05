// Project members: Tania Jaswal and Jasmine Wells
// File name: swampcooler.o

//included libraries

#include <Stepper.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <SimpleDHT.h>
#include <TimeLib.h>

#define w_threshold 130 // water threshold
#define t_threshold 20 // temperature threshold

#define IN1 7
#define IN2 6
#define IN3 5
#define IN4 4

//initialize lcd screen 
LiquidCrystal lcd(7,8,9,10,11,12);


// Port Registers

// LEDs (Output)
volatile unsigned char* myPORT_B = (unsigned char*) 0x25; 
volatile unsigned char* myDDR_B  = (unsigned char*) 0x24;
volatile unsigned char* myPIN_B  = (unsigned char*) 0x23;

// Temperature and Humdity (Input)
volatile unsigned char* myPORT_C = (unsigned char*) 0x28;
volatile unsigned char* myDDR_C  = (unsigned char*) 0x27;
volatile unsigned char* myPIN_C  = (unsigned char*) 0x26;

// Fan Motor
volatile unsigned char* myPORT_E = (unsigned char*) 0x2E; 
volatile unsigned char* myDDR_E  = (unsigned char*) 0x2D;
volatile unsigned char* myPIN_E  = (unsigned char*) 0x2C;

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
// volatile unsigned char* myADCSRA = (unsigned char*) 0x7A;
// volatile unsigned char* myADCSRB = (unsigned char*) 0x7B;
// volatile unsigned char* myADMUX  = (unsigned char*) 0x7C;
// volatile unsigned int* myADCDATA = (unsigned int*)  0x78;

// Pin change Interrupt Registers
volatile unsigned char* myPCMSK1 = (unsigned char *) 0x6C;
volatile unsigned char* myPCICR  = (unsigned char *) 0x68;




float temperature = 0;
float humidity = 0;

unsigned int state_count = 0;

//vent variables
const int steps_per_rev = 32; 
Stepper motor(steps_per_rev, IN1, IN3, IN2, IN4);
int Pval = 0;
int potVal = 0;

// Functions

void idle_state(int water_level, float temperature1);

void error_state(int water_level, float temperature1);

void running_state(int water_level, float temperature1);

void disabled_mode();

float lcd_display (float temperature1, float humidity);

void Vent_control();

void setup() {
  
    motor.setSpeed(10);
    Serial.begin(9600);

}

void loop() {

  //step motor
  Vent_control();

    // LCD Display

    lcd.clear();
    lcd.print("Temp: ");        
    lcd.print(temperature);     // Displays Temperature Value 
    lcd.print("C");             // prints "C" for Celsius

    lcd.print("Humidity: ");
    lcd.print(humidity);        // Displays Humidity Value 
    lcd.print("%");


}



// Define Functions

void idle_state(int water_level, float temperature1){

  if(water_level > w_threshold && temperature < t_threshold){
        
    // LEDs
    *myPORT_B &=  0x00;               // Turn all LEDs off
    *myPORT_B |=  0x80;               // Turn on GREEN LED

    // Turn Motor off
    *myPORT_B |= 0x08;        
    *myPORT_B &= 0xFD;        
  }
}


void error_state(int water_level, float temperature1){
  if(water_level <= w_threshold){
    
     // LEDs
     *myPORT_B &=  0x00;               // Turn all LEDs off
     *myPORT_B |=  0x20;               // Turn on RED LED

    // Turn Motor off
    *myPORT_B |= 0x08;        
    *myPORT_B &= 0xFD;       
  }
}

void running_state(int water_level, float temperature1){
if(water_level > w_threshold && temperature > t_threshold){
    
    // LEDs
    *myPORT_B &=  0x00;               // Turn all LEDs off
    *myPORT_B |=  0x40;               // Turn on BLUE LED

    // Turn Motor ON
    *myPORT_B |= 0x08;        
    *myPORT_B |= 0x02;        
  }
}

void disabled_mode(){

}

float lcd_display (float temperature1, float humidity){

}


void Vent_control(){
  potVal = map(analogRead(A0),0,1024,0,500);
  
  if(potVal>Pval)
    motor.step(5);
  if(potVal<Pval)
    motor.step(-5);
 Pval = potVal;
}