void Write_Pin(volatile unsigned char* Port,unsigned char Pin,unsigned State);
void Pin_Mode(volatile unsigned char* Port, unsigned char Pin,bool INOUT);

// Port C
volatile unsigned char* myPORT_C = (unsigned char*) 0x28;
volatile unsigned char* myDDR_C  = (unsigned char*) 0x27;
volatile unsigned char* myPIN_C  = (unsigned char*) 0x26;

// Port D
volatile unsigned char* myPORT_D = (unsigned char*) 0x2B;
volatile unsigned char* myDDR_D  = (unsigned char*) 0x2A;
volatile unsigned char* myPIN_D  = (unsigned char*) 0x29;

//Port G
volatile unsigned char* myPORT_G = (unsigned char*) 0x34;
volatile unsigned char* myDDR_G  = (unsigned char*) 0x33;
volatile unsigned char* myPIN_G  = (unsigned char*) 0x32;

#define ENABLE 36
#define DIRB 38
#define DIRA 40

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  *myDDR_C |= 0x02;    //enable
  Pin_Mode(*myDDR_G,1,false);   //DIRA
  Pin_Mode(*myDDR_D,7,false);   //DIRB
  
}

void loop() {
  // put your main code here, to run repeatedly:
    *myPORT_C |= 0x02; // enable on
    *myPORT_G |= 0x02;  //one way

}

void Pin_Mode(volatile unsigned char* Port, unsigned char Pin,bool INOUT){
  if (INOUT == false){  
     Serial.print("WE IN");          //If output
    *Port |= (0x01 << Pin);
    Serial.println(*myDDR_C);         //Shift over to pin and set to 1
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
    Serial.print("WE IN");
    *Port |= 0x01 << Pin;         //Shoft over to pin and set to 1
  }
}