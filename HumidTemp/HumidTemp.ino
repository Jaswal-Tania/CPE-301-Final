#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN 6
#define DHTTYPE DHT11

DHT DHT(DHTPIN,DHTTYPE);
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
 DHT.begin();// initialize the sensor
 lcd.begin(16, 2);
}
void loop() {
  int check = DHT.read(DHTPIN);
 lcd.clear();
   lcd.setCursor(0,0);// set the cursor on the first row and column
   lcd.print("Humidity=");
   lcd.print((float)DHT.readHumidity());//print the humidity
   lcd.print("%");
   lcd.setCursor(0,1);//set the cursor on the second row and first column
   lcd.print("Temp=");
   lcd.print((float)DHT.readTemperature());//print the temperature
   lcd.print("Celsius");
   delay(2000);
   lcd.clear();
}
