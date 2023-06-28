#include "DHT.h"

#define DHTTYPE DHT22
#define dht_pin D4

DHT dht(dht_pin, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  delay(2000);
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if(isnan(h) || isnan(t)){
    Serial.println("DHT read Failed!");
    
    return;
  }
  float hic = dht.computeHeatIndex(t,h,false);
  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print(", Humidity: ");
  Serial.print(h);
  Serial.print(", HIC: ");
  Serial.println(hic);
}
