#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor

#define DHTTYPE DHT22   // DHT 11
#define dht_dpin 0

String apiKey = "F3B5SDC8JXTYMBVH";

const char* ssid = "wifi-sa-bahay-V2.4";
const char* password = "wifi-sa-bahay-V2.4-password";

const char* serverName = "http://api.thingspeak.com/update";
DHT dht(dht_dpin, DHTTYPE); 
void setup(void)
{ 
  dht.begin();
  Serial.begin(115200);
  connectToWifi();
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 

  Serial.print("Temp: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);

  // try to post data until it sends
  int response = 1;
  do{
    response = postData(t, h);
    if(response == -1){
    }else if(response == 1){
      // reconnect to wifi
      connectToWifi();
    }
  }while(response != 200);
  ESP.deepSleep(600e6);
}
void loop() {    
}

void connectToWifi(){
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }
}


int postData(float temp, float humid){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "api_key="+apiKey+"&field1="+temp+"&field2="+humid;

    int httpResponseCode = http.POST(httpRequestData);
    return httpResponseCode;
  }
  return 1;
}
