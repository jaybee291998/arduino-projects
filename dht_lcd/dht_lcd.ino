#include<Wire.h>
#include<LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTTYPE DHT11

#define dht_pin 4

LiquidCrystal_I2C lcd(0x27, 16, 2);

DHT dht(dht_pin, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  lcd.begin();
  dht.begin();
}

void loop() {
  delay(2000);
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if(isnan(h) || isnan(t)){
    lcd.clear();
    lcd.print("DHT read Failed!");
    delay(500);
    lcd.clear();
    
    return;
  }
  float hic = dht.computeHeatIndex(t,h,false);

  lcd.clear();
  lcd.print("T:");
  lcd.print(t);
  lcd.print(", H:");
  lcd.print(h);
  lcd.setCursor(0,1);
  lcd.print("HIC: ");
  lcd.print(hic);
}
