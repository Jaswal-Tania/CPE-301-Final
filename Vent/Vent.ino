#include <Stepper.h>

#define IN1 42
#define IN2 44
#define IN3 46
#define IN4 48

//vent variables
const int steps_per_rev = 32; 
Stepper motor(steps_per_rev, IN1, IN3, IN2, IN4);
int Pval = 0;
int potVal = 0;

void Vent_control();

void setup() {
  // put your setup code here, to run once:
  motor.setSpeed(200);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 //step motor
  Vent_control();
}

void Vent_control(){
  potVal = map(analogRead(A0),0,1024,0,500);
  
  if(potVal>Pval)
    motor.step(5);
  if(potVal<Pval)
    motor.step(-5);
 Pval = potVal;
}
