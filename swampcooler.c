
//Project members: Tania Jaswal and Jasmine Wells
// File name: swampcooler.o

//included libraries

#include <Stepper.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <DHT.h>
#include <RTClib.h>

#define w_threshold 130 // water threshold
#define t_threshold 20 // temperature threshold

#define DHTPIN 36 // DHT Pin
#define SERVO_PIN A1
#define DHTTYPE DHT11

#define ON true
#define OFF false
#define Output false
#define Input true

#define IN1 42
#define IN2 44
#define IN3 46
#define IN4 48


// Port Registers

// Port B
volatile unsigned char* myPORT_B = (unsigned char*) 0x25; 
volatile unsigned char* myDDR_B  = (unsigned char*) 0x24;
volatile unsigned char* myPIN_B  = (unsigned char*) 0x23;

// Port C
volatile unsigned char* myPORT_C = (unsigned char*) 0x28;
volatile unsigned char* myDDR_C  = (unsigned char*) 0x27;
volatile unsigned char* myPIN_C  = (unsigned char*) 0x26;

// Port E
volatile unsigned char* myPORT_E = (unsigned char*) 0x2E; 
volatile unsigned char* myDDR_E  = (unsigned char*) 0x2D;
volatile unsigned char* myPIN_E  = (unsigned char*) 0x2C;

// Port F
volatile unsigned char* myPORT_F = (unsigned char*) 0x31;
volatile unsigned char* myDDR_F  = (unsigned char*) 0x30;
volatile unsigned char* myPIN_F  = (unsigned char*) 0x2F;

// Port H
volatile unsigned char* myPORT_H = (unsigned char*) 0x102;
volatile unsigned char* myDDR_H  = (unsigned char*) 0x101;
volatile unsigned char* myPIN_H  = (unsigned char*) 0x100;

// Port K
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
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

// Pin change Interrupt Registers
volatile unsigned char* myPCMSK1 = (unsigned char *) 0x6C;
volatile unsigned char* myPCICR  = (unsigned char *) 0x68;

DateTime now;
DHT DHT(DHTPIN, DHTTYPE);
Servo servo;
RTC_DS1307 rtc;

//initialize lcd screen 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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

// Functions

void idle_state(int water_level, float temperature1);
void error_state(int water_level, float temperature1);
void running_state(int water_level, float temperature1);
void disabled_mode();
float lcd_display (float temperature1, float humidity);
void printTime();
void Vent_control();
void Write_Pin_State(volatile unsigned char* Port,unsigned char Pin,unsigned State);
void Set_Pin(volatile unsigned char* Port, unsigned char Pin,bool INOUT);
int Read_Water_Level();
unsigned int adc_read(unsigned char adc_channel_num);
void adc_init();


void setup() {
  
  Set_Pin(myDDR_B,3,Output);
  Write_Pin_State(myDDR_B,3,ON);
  Serial.begin(9600);
  Set_Pin(myDDR_H,5,Output);
  adc_init();
 
  motor.setSpeed(200);
  
  DHT.begin();
  servo.attach(SERVO_PIN);
  lcd.begin(16,2);

  rtc.begin();
  if (!rtc.isrunning()){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  *myDDR_B |= 0xFF;        
  
  *myDDR_K &= 0x00;         
  
  *myPORT_K |= 0xFF;     

  *myDDR_F &= 0xFF;    
  
  *myPORT_F |= 0x80;      

  
  *myDDR_E &= 0x10;         
  
  *myPORT_E |= 0x10;        

  // LCD size
lcd.print("123456789");
}

void loop() {
 //water
  if(Read_Water_Level()>580){

   Write_Pin_State(myPORT_H,5,ON);
  }
  else{
    Write_Pin_State(myPORT_H,5,OFF);
    Serial.print("Water too low");
  }
  //step motor
  Vent_control();
  
  if (!(*myPIN_K & 0x40)){ // Checks whether the button is pushed; checks bit 6 (0100 0000)
    
    // Checks again if the button is pressed
    if (!(*myPIN_K & 0x40))
    {
      state_count++;
      state_count %= 2;
      while (!(*myPIN_H & 0x40));
    }
  }

  if(state_count == 0)
  {
    disabled_mode();
  }
 
  else
  {
    // Water Level Reading
    
    
    // Temperature & Humidity Sensor Reading
    int check = DHT.read(DHTPIN);
    temperature = (float)DHT.readTemperature();
    humidity = (float)DHT.readHumidity();
    
  
    // LCD Display
    lcd_display(temperature, humidity);


    // Call Functions
    idle_state(water_level, temperature);
    error_state(water_level, temperature);
    running_state(water_level, temperature);
  }

}



// Define Functions

void idle_state(int water_level, float temperature1){

  if(water_level > w_threshold && temperature < t_threshold){

    lcd.print("**Idle**");   
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
    
    lcd.print("**Error**");

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
    
    lcd.print("**Running**");

    // LEDs
    *myPORT_B &=  0x00;               // Turn all LEDs off
    *myPORT_B |=  0x40;               // Turn on BLUE LED

    // Turn Motor ON
    *myPORT_B |= 0x08;        
    *myPORT_B |= 0x02;        
  }
}

void disabled_mode(){

  lcd.print("**Disabled**");
  // Clear the LCD
  lcd.clear();

  // LEDs
  *myPORT_B &=  0x00;               // Turn all LEDs off
  *myPORT_B |=  0x10;               // Turn on YELLOW LED

  // Turn Motor Off
  *myPORT_B |= 0x08;        
  *myPORT_B &= 0xFD; 
}

float lcd_display (float temperature1, float humidity){

  lcd.clear();
    lcd.print("Temp: ");        
    lcd.print(temperature1);     // Displays Temperature Value 
    lcd.print("C");             // prints "C" for Celsius

    lcd.print("Humidity: ");
    lcd.print(humidity);        // Displays Humidity Value 
    lcd.print("%");

}

void printTime()
{
  DateTime now = rtc.now();
  if (state_count == 1)
  {
    lcd.print("\n");
    lcd.print("Enabled: (");
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print(") ");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    lcd.println();
  }
  else if (state_count == 0)
  {
    lcd.print("\n");
    lcd.print("Disabled: (");
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print(") ");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    lcd.println();
  }
}

void Vent_control(){
  potVal = map(analogRead(A0),0,1024,0,500);  //map the potimeter to analog 1
  
  if(potVal>Pval)                         //if the read value is bigger than previous value
    motor.step(5);                        //move motor 5 steps
  if(potVal<Pval)                         //if the read vlaue is less than the previous value
    motor.step(-5);                       //move motor back 5 steps
    
  Pval = potVal;                          //set the previous value to the read value
}

int Read_Water_Level(){
  
  Write_Pin_State(myPORT_B,3,ON);   //Turn the water sensor ON
  val = adc_read(15);               //Read the analog value from sensor
  Write_Pin_State(myPORT_B,3,OFF);  //Turn the water sensor OFF
  Serial.print("Water level: ");
  Serial.println(val);
  return val;                       //Send current reading
}

void Set_Pin(volatile unsigned char* Port, unsigned char Pin,bool INOUT){
  if (INOUT == false){             //If output
    *Port |= 0x01 << Pin;          //Shift over to pin and set to 1
  }
  else{                           //If input
    *Port &= ~(0x01 << Pin);       //Shift over to pin and set to 0
  }
}

void Write_Pin_State(volatile unsigned char* Port,unsigned char Pin,unsigned State){
  if(State == 0)                  //If state off
  {
    *Port &= ~(0x01 << Pin);      //Shift over to pin and set to 0
  }
  else                            //If state on
  {
    *Port |= 0x01 << Pin;         //Shoft over to pin and set to 1
  }
}
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