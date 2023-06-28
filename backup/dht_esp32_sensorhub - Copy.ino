#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#include <Adafruit_BMP085.h>
#define seaLevelPressure_hPa 1013.25

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 4
#define photo_pin 36
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */

int time_before_next_sample = 1 * 60 * 1000;

float h;
float t;
float p;
float l;

byte max_tries = 20;

String apiKey = "qtKrIzIfrBBHxuihlhP2yTLwyyh3AfFC";

const char* ssid = "wifi-sa-bahay-V2.4";
const char* password = "wifi-sa-bahay-V2.4-new-password";

const char* serverName = "http://sensorhub.pythonanywhere.com/channel/update/";
DHT dht(dht_dpin, DHTTYPE); 
Adafruit_BMP085 bmp;
void setup(void)
{ 
  dht.begin();
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("BMP180 Not Found. CHECK CIRCUIT!");
  while (1) {}
  }
  connectToWifi();
}
void loop() {   
  do{
    h = dht.readHumidity();

    Serial.print("Humidity: ");
    Serial.println(h);
//    delay(1000);
  }while(isnan(h) || isnan(t));
  // Check if any reads failed and exit early (to try again).
  // Compute heat index in Celsius (isFahreheit = false)
  t = bmp.readTemperature();
  while(t >= 50){
    t = bmp.readTemperature();
  }
  p = bmp.readPressure();
  l = analogRead(photo_pin);
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Pressure: ");
  Serial.println(p);
  Serial.print("light: ");
  Serial.println(l);
  
  
  // try to post data until it sends
  int response = 1;
  do{
    response = postData(t, h, p, l);
    if(response == -1){
    }else if(response == 1){
      // reconnect to wifi
      connectToWifi();
    }
  }while(response != 200);
  delay(time_before_next_sample);
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


int postData(float temp, float humid, float pres, float light){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "api_key="+apiKey+"&temperature="+temp+"&humidity="+humid+"&pressure="+pres+"&light="+light;

    int httpResponseCode = http.POST(httpRequestData);
    String response = http.getString();
    http.end();
//    Serial.print("Request Data: ");
//    Serial.println(httpRequestData);
    Serial.print("Respone Code: ");
    Serial.println(httpResponseCode);
//    Serial.print("Respone: ");
//    Serial.println(response);
    return httpResponseCode;
  }
  return 1;
}
