#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor

#define DHTTYPE DHT22   // DHT 11
#define dht_dpin 4

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600        /* Time ESP32 will go to sleep (in seconds) */

byte max_tries = 20;

float h;
float t;

String apiKey = "F3B5SDC8JXTYMBVH";

const char* ssid = "wifi-sa-bahay-V2.4";
const char* password = "wifi-sa-bahay-V2.4-password";

const char* serverName = "http://api.thingspeak.com/update";
DHT dht(dht_dpin, DHTTYPE); 
void setup(void)
{ 
  dht.begin();
  connectToWifi();
  do{
    h = dht.readHumidity();
    t = dht.readTemperature();
  }while(isnan(h) || isnan(t));
  float heat_index = dht.computeHeatIndex(t, h, false);  
  // try to post data until it sends
  int response = 1;
  do{
    response = postData(t, h, heat_index);
    if(response == -1){
    }else if(response == 1){
      // reconnect to wifi
      connectToWifi();
    }
  }while(response != 200);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
void loop() {    
}

void connectToWifi(){
  byte connection_tries = 0;
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
    if(connection_tries >= max_tries){
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      esp_deep_sleep_start();
    }
    connection_tries++;
  }
}


int postData(float temp, float humid, float hic){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "api_key="+apiKey+"&field1="+temp+"&field2="+humid+"&field3="+hic;

    int httpResponseCode = http.POST(httpRequestData);
    http.end();
    return httpResponseCode;
  }
  return 1;
}
