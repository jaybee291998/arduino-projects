#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor

#define DHTTYPE DHT22   // DHT 11
#define dht_dpin 4

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600        /* Time ESP32 will go to sleep (in seconds) */

byte max_tries = 20;

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
  float h;
  float t;
  do{
    h = dht.readHumidity();
    t = dht.readTemperature();

    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("Temperature: ");
    Serial.println(t);
  }while(isnan(h) || isnan(t));
  // Check if any reads failed and exit early (to try again).
  // Compute heat index in Celsius (isFahreheit = false)
  float heat_index = dht.computeHeatIndex(t, h, false);
  Serial.print("Heat Indes: ");
  Serial.println(heat_index);

  
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
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
  delay(100);
  esp_deep_sleep_start();
}
void loop() {    
}

void connectToWifi(){
  byte connection_tries = 0;
  Serial.println("Connecting");
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
  Serial.println("");
  Serial.print("Conencted to WiFi network with IP address: ");
  Serial.println(WiFi.localIP());
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
    Serial.print("Request Data: ");
    Serial.println(httpRequestData);
    Serial.print("Respone Code: ");
    Serial.println(httpResponseCode);
    return httpResponseCode;
  }
  return 1;
}
