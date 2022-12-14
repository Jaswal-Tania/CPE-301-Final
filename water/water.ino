
// Button/Transistor
volatile unsigned char* myPORT_K = (unsigned char*) 0x108;
volatile unsigned char* myDDR_K  = (unsigned char*) 0x107;
volatile unsigned char* myPIN_K  = (unsigned char*) 0x106;


//digital pin
volatile unsigned char* myPORT_B = (unsigned char*) 0x25; 
volatile unsigned char* myDDR_B  = (unsigned char*) 0x24;
volatile unsigned char* myPIN_B  = (unsigned char*) 0x23;

volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;
int val = 0;

void setup() {
  // put your setup code here, to run once:
  *myDDR_B |= 0x08;
  *myPORT_B &= 0x08;
  Serial.begin(9600);
  pinMode(8,OUTPUT);
  adc_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  int level = readSensor();
  if(readSensor()>200){

   digitalWrite(8, HIGH);
  }
  else{
    digitalWrite(8,LOW);
  }

  

}

int readSensor() {
  *myPORT_B |= 0x08;  // Turn the sensor ON
  val = adc_read(15);    // Read the analog value form sensor
  Serial.print("Water level: ");
  Serial.println(val);
  return val;             // send current reading
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
