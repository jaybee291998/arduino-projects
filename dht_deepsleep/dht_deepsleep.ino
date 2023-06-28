#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor

#define DHTTYPE DHT11   // DHT 11
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
  Serial.println("Humidity and temperature\n\n");
  delay(700);    float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t); 
  Serial.println("C  ");

  // try to post data until it sends
  int response = 1;
  do{
    response = postData(t, h);
    if(response == -1){
      Serial.println("Failed to post data");
    }else if(response == 1){
      Serial.println("Not connected to wifi");
      // reconnect to wifi
      connectToWifi();
    }
    Serial.print("HTTP Response Code: ");
    Serial.println(response);
  }while(response != 200);

  Serial.println("Now sleeping for 30 seconds");
  ESP.deepSleep(30e6);
}
void loop() {    
}

void connectToWifi(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to wifi network with IP address: ");
  Serial.println(WiFi.localIP());
}


int postData(float temp, float humid){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "api_key="+apiKey+"&field1="+temp+"&field2="+humid;

    int httpResponseCode = http.POST(httpRequestData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    return httpResponseCode;
  }
  return 1;
}
