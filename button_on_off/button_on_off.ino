// Port B
volatile unsigned char* myPORT_B = (unsigned char*) 0x25; 
volatile unsigned char* myDDR_B  = (unsigned char*) 0x24;
volatile unsigned char* myPIN_B  = (unsigned char*) 0x23;
// Port H
volatile unsigned char* myPORT_H = (unsigned char*) 0x102;
volatile unsigned char* myDDR_H  = (unsigned char*) 0x101;
volatile unsigned char* myPIN_H  = (unsigned char*) 0x100;
#define ON true
#define OFF false
#define Output false
#define Input true

void Write_Pin_State(volatile unsigned char* Port,unsigned char Pin,unsigned State);
void Set_Pin(volatile unsigned char* Port, unsigned char Pin,bool INOUT);

int state_count = 0;

void setup() {
  // put your setup code here, to run once:
  //pinMode(52,INPUT);
  //pinMode(7,OUTPUT);
  Serial.begin(9600);
  Set_Pin(myDDR_B,1,Input);
  Set_Pin(myDDR_H,4,Output);
  Write_Pin_State(myPORT_H,4,OFF);

}

void loop() {
  // put your main code here, to run repeatedly:
  int button = *myPIN_B;
   button = button & 0x02;
if ((button)!= 0 ){ // Checks whether the button is not being pushed; checks bit 6 (0100 0000)
     Serial.println(state_count);
      state_count = !state_count;
Serial.println(state_count);      
   
}

if(state_count==0){
  Serial.print("   HEYYO    ");

}
else{
  Serial.print("BONN");
}
  //if(state_count == 0)
  //{
    //digitalWrite(8,HIGH);
    //Write_Pin_State(myPORT_H,4,OFF);
    //disabled_mode();
  //}
 
  //else{
    //digitalWrite(8,LOW);
     //Write_Pin_State(myPORT_H,4,ON);
  //}
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
