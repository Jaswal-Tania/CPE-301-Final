
//Project members: Tania Jaswal and Jasmine Wells
// File name: swampcooler.o

//included libraries
#include <Stepper.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <DHT.h>
#include <RTClib.h>

// Port Registers

// Port B
volatile unsigned char* myPORT_B = (unsigned char*) 0x25; 
volatile unsigned char* myDDR_B  = (unsigned char*) 0x24;
volatile unsigned char* myPIN_B  = (unsigned char*) 0x23;

// Port C
volatile unsigned char* myPORT_C = (unsigned char*) 0x28;
volatile unsigned char* myDDR_C  = (unsigned char*) 0x27;
volatile unsigned char* myPIN_C  = (unsigned char*) 0x26;

// Port D
volatile unsigned char* myPORT_D = (unsigned char*) 0x2B;
volatile unsigned char* myDDR_D  = (unsigned char*) 0x2A;
volatile unsigned char* myPIN_D  = (unsigned char*) 0x29;

// Port E
volatile unsigned char* myPORT_E = (unsigned char*) 0x2E; 
volatile unsigned char* myDDR_E  = (unsigned char*) 0x2D;
volatile unsigned char* myPIN_E  = (unsigned char*) 0x2C;

// Port F
volatile unsigned char* myPORT_F = (unsigned char*) 0x31;
volatile unsigned char* myDDR_F  = (unsigned char*) 0x30;
volatile unsigned char* myPIN_F  = (unsigned char*) 0x2F;

//Port G
volatile unsigned char* myPORT_G = (unsigned char*) 0x34;
volatile unsigned char* myDDR_G  = (unsigned char*) 0x33;
volatile unsigned char* myPIN_G  = (unsigned char*) 0x32;

// Port H
volatile unsigned char* myPORT_H = (unsigned char*) 0x102;
volatile unsigned char* myDDR_H  = (unsigned char*) 0x101;
volatile unsigned char* myPIN_H  = (unsigned char*) 0x100;

// Port K
volatile unsigned char* myPORT_K = (unsigned char*) 0x108;
volatile unsigned char* myDDR_K  = (unsigned char*) 0x107;
volatile unsigned char* myPIN_K  = (unsigned char*) 0x106;

//Port L
volatile unsigned char* myPORT_L = (unsigned char*) 0x10B;
volatile unsigned char* myDDR_L  = (unsigned char*) 0x10A;
volatile unsigned char* myPIN_L  = (unsigned char*) 0x109;

// Timer Registers
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIFR1  = (unsigned char *) 0x36;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;

// ADC Registers 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

// Pin change Interrupt Registers
volatile unsigned char* myPCMSK1 = (unsigned char *) 0x6C;
volatile unsigned char* myPCICR  = (unsigned char *) 0x68;

// Functions
void idle_state();
void error_state();
void running_state();
void disabled_mode();
void printTime();
void Vent_control();
void Write_Pin(volatile unsigned char* Port,unsigned char Pin,unsigned State);
void Pin_Mode(volatile unsigned char* Port, unsigned char Pin,bool INOUT);
int Read_Water_Level();
unsigned int adc_read(unsigned char adc_channel_num);
void adc_init();
void Check_Water();
void Humit_Temp_Read_Print();
void Turn_Off_All_Lights();
void Turn_On(char color);
void Fan_ON_OFF(unsigned int state);
void clock_setup();

#define ON true
#define OFF false
#define Output false
#define Input true

#define IN1 42
#define IN2 44
#define IN3 46
#define IN4 48

#define w_threshold 130 // water threshold
#define t_threshold 22 // temperature threshold

#define SERVO_PIN A1
#define DHTPIN 6 // DHT Pin
#define DHTTYPE DHT11


//DHT for humidity and lcd
DHT DHT(DHTPIN,DHTTYPE);

//initialize lcd screen 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//humity and temp and water
float temperature = 0;
float humidity = 0;
float water_level = 0;

unsigned int state_count = 0;

//vent variables
const int steps_per_rev = 32; 
Stepper motor(steps_per_rev, IN1, IN3, IN2, IN4);
int Pval = 0;
int potVal = 0;
int val = 0;

//variables for on off button
int count = 0;
int StateCount = 1;

//variables for clock
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//SETUP
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void setup() {
  
  //begin the serial
  Serial.begin(9600);
  
  //PinModes
  Pin_Mode(myDDR_B,1,Output);
  Pin_Mode(myDDR_B,3,Output);
  Pin_Mode(myDDR_H,5,Output);
  Pin_Mode(myDDR_C,1,Output);
  Pin_Mode(myDDR_G,1,Output);
  Pin_Mode(myDDR_D,7,Output);
  Pin_Mode(myDDR_B,1,Input);
  Pin_Mode(myDDR_H,4,Output);

  //initialize pins
  Write_Pin(myDDR_B,3,ON);

  //adc initialization
  adc_init();

 //Vent initalization
  motor.setSpeed(200);
  
  //start the DHT for lcd and humity
  DHT.begin(); 
  lcd.begin(16, 2);   

  //clock
  clock_setup();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//LOOP
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void loop() {
  int button = *myPIN_B;
  button = button & 0x02;
  if(button != 0){                    //if high/button is pressed
      switch(StateCount){
        case 0:                       //if in disable mode
        //count = 0;
        idle_state();                   //NEED TO CHANGE TO IDLE
        printTime();                  //print time it was disabled
        break;
        case 1:
        disabled_mode();              //go to disable mode
        printTime();                  //print time it was disabled
        break;
        case 2:
        //count = 0;     
        disabled_mode();               //disable mode on            
        printTime(); 
        break;
        case 3: 
        //count = 0;
        disabled_mode(); 
        printTime(); 
        break; 
        delay(100);    
      }
      
  } 
  else{                           //if button hasnt been pressed
    switch(StateCount){
      case 0:                     //supposed to be in disable mode
        disabled_mode();          //turn on disabled mode
        break;
      case 1:                     //supposed to be in running state
        running_state();          //turn on running state
        break;
      case 2:                     //supposed to be in idle
        idle_state();             //go to idle state
      break;
      case 3:                     //suppoe to be in error
        error_state();            //go in error
      break;      
    }
  }   


}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//HELPER FUNCTIONS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//MODES
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void running_state(){
    //make sure state is correct
    StateCount = 1;

    // LEDs
    Turn_Off_All_Lights();
    Turn_On('B');              // Turn on BLUE LED 

    if(count>5||count == 0){
    //check temp and humity print to lcd
        Humit_Temp_Read_Print(); 
        count++;
        if(count == 5){
            count = 1;      
        }
    }

    //water level check
    Check_Water();

    //fan running
    Fan_ON_OFF(ON);
    //step motor
    Vent_control();  
    
}


void idle_state(){
  //state check
  StateCount = 2;

    // LEDs
    Turn_Off_All_Lights();
    Turn_On('G');              // Turn on Green LED 

    if(count>5||count == 0){
    //check temp and humity print to lcd
        Humit_Temp_Read_Print(); 
        count++;
        if(count == 5){
            count = 1;      
        }
    }
    
    //water level check
    Check_Water();

    //fan running
    Fan_ON_OFF(OFF);
    //step motor
    Vent_control(); 
  
}


void error_state(){
    //check state
    StateCount == 3;

    //error message
    lcd.print("**WATER level is too Low**");

    // LEDs
    Turn_Off_All_Lights();
    Turn_On('R');              // Turn on Red LED 

    //fan
    Fan_ON_OFF(OFF); 
          
      if(count>5||count == 0){
    //check temp and humity print to lcd
        Humit_Temp_Read_Print(); 
        count++;
        if(count == 5){
            count = 1;      
        }
    }

    //water level check
    Check_Water();    

}



void disabled_mode(){
  //state check
  StateCount = 0;

  // LEDs
  Turn_Off_All_Lights();    // Turn all LEDs off
  Turn_On('Y');             // Turn on YELLOW LED

  // Turn Motor Off
  Fan_ON_OFF(OFF); 
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//TIME
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void printTime()
{
  DateTime now = rtc.now();
  if (state_count == 1)
  {
    Serial.print("\n");
    Serial.print("Enabled: (");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  }
  else if (state_count == 0)
  {
    Serial.print("\n");
    Serial.print("Disabled: (");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  }
  
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//VENT CONTROLS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Vent_control(){
  potVal = map(analogRead(A0),0,1024,0,500);  //map the potimeter to analog 1
  
  if(potVal>Pval)                         //if the read value is bigger than previous value
    motor.step(5);                        //move motor 5 steps
  if(potVal<Pval)                         //if the read vlaue is less than the previous value
    motor.step(-5);                       //move motor back 5 steps

  Pval = potVal;                          //set the previous value to the read value
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//HUMIDITY AND TEMPERATURE READING
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 void Humit_Temp_Read_Print(){

  
    int check = DHT.read(DHTPIN);
    int CheckTemp =DHT.readTemperature();
    Serial.println(CheckTemp);
    lcd.clear();
    lcd.setCursor(0,0);// set the cursor on the first row and column
    lcd.print("Humidity=");
    lcd.print((float)DHT.readHumidity());//print the humidity
    lcd.print("%");
    lcd.setCursor(0,1);//set the cursor on the second row and first column
    lcd.print("Temp=");
    lcd.print((float)CheckTemp);//print the temperature
    lcd.print("Celsius");
    delay(2000);

    if(CheckTemp <= t_threshold){
      Serial.print("WE SHOULD GO TO RUNNING MODE HERE");
      running_state();
      printTime();       
    }
    else if((CheckTemp > t_threshold)){
      idle_state();
      printTime(); 
    }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//WATER LEVEL STUFF
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int Read_Water_Level(){
  
  Write_Pin(myPORT_B,3,ON);   //Turn the water sensor ON
  val = adc_read(15);               //Read the analog value from sensor
  Write_Pin(myPORT_B,3,OFF);  //Turn the water sensor OFF
  Serial.print("Water level: ");
  Serial.println(val);
  return val;                       //Send current reading
}
void Check_Water(){  
   // Water Level Reading
    if( (Read_Water_Level() < w_threshold)  ){
     error_state();
    }
    else{
      idle_state();
    }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FAN STUFF
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Fan_ON_OFF(unsigned int state){
  if(state==1){
    Write_Pin(myPORT_C,1,ON);
    Write_Pin(myPORT_G,1,ON);
  }
  else{
    Write_Pin(myPORT_C,1,OFF);
    Write_Pin(myPORT_G,1,OFF);
  }
 
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~4
//Clock
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void clock_setup(){
  #ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~4
//LIGHTS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Turn_Off_All_Lights(){
  Write_Pin(*myPORT_B,4,OFF); //Turn off Blue
  Write_Pin(*myPORT_H,6,OFF); //Turn off Green
  Write_Pin(*myPORT_H,5,OFF); //Turn off Yellow 
  Write_Pin(*myPORT_H,4,OFF); //Turn off red
}
void Turn_On(char color){
  switch (color){
    case 'B':
        Write_Pin(myPORT_B,4,ON);  //Turn on Blue
        break;
    case 'G':
        Write_Pin(myPORT_H,6,ON);  //Turn on Green
        break;
    case 'Y':
    Serial.print("YELLOW");
        Write_Pin(myPORT_H,5,ON); //Turn on Yellow 
        break;
    case 'R':
        Write_Pin(myPORT_H,4,ON); //Turn on red
        break;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//PINMODE FUNCTIONS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Pin_Mode(volatile unsigned char* Port, unsigned char Pin,bool INOUT){
  if (INOUT == false){             //If output
    *Port |= 0x01 << Pin;          //Shift over to pin and set to 1
  }
  else{                           //If input
    *Port &= ~(0x01 << Pin);       //Shift over to pin and set to 0
  }
}

void Write_Pin(volatile unsigned char* Port,unsigned char Pin,unsigned State){
  if(State == 0)                  //If state off
  {
    *Port &= ~(0x01 << Pin);      //Shift over to pin and set to 0
  }
  else                            //If state on
  {
    *Port |= 0x01 << Pin;         //Shoft over to pin and set to 1
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//ADC STUFF
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void adc_init()
{
  // setup the A register
  *my_ADCSRA |= 0x80; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0xEF; // clear bit 5 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0xF7; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0xC0; // clear bit 5 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0xF7; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0xF8; // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0x7F; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0x40; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0xDF; // clear bit 5 to 0 for right adjust result
  //@*my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0xE0; // clear bit 4-0 to 0 to reset the channel and gain bits
}
unsigned int adc_read(unsigned char adc_channel_num)
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0xE0; 
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0xF7;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0x08;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}
