#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#include <Adafruit_BMP085.h>
#define seaLevelPressure_hPa 1013.25

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 4
#define photo_pin 36
#define motion_pin 23
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */

int time_before_next_sample = 1 * 60 * 1000;
boolean motion_detected = false;
int MAX_COUNT = 100000000;
int count;

long motion_start=0;
long now;
long previous = 0;
//long interval = ;
float h;
float t;
float p;
float l;
float total_light = 0;
float average_light = 0;
float number_of_samples = 0;

byte max_tries = 20;

String apiKey = "hNIwd7V37WnmXcURquKQlGTSpfoMf6Cl";
String apiKey2 = "jtRRyeAjAb1712hpWKZnEAo3JHtrZF0U";

const char* ssid = "wifi-sa-bahay-V2.4";
const char* password = "wifi-sa-bahay-V2.4-new-password";

const char* serverName = "http://sensorhub.pythonanywhere.com/channel/update/";
DHT dht(dht_dpin, DHTTYPE); 
Adafruit_BMP085 bmp;
void setup(void)
{ 
  count = 0;
  dht.begin();
  Serial.begin(115200);
  Serial.println("Hwllo");
  if (!bmp.begin()) {
  Serial.println("BMP180 Not Found. CHECK CIRCUIT!");
  while (1) {}
  }
  connectToWifi();
}
void loop() {   
  now = millis();
  if((now - previous) > time_before_next_sample){
    previous = millis();
//    Serial.println("ZEN");
    collectSample();
  }
  motionDetection();
}

void motionDetection(){
  int motion = digitalRead(motion_pin);
  if(motion == HIGH){
    total_light += analogRead(photo_pin);
    number_of_samples++;
    if(!motion_detected){
      motion_detected = true;
      motion_start = millis();
      Serial.println("Motion Detected");
    }
  }else{
    if(motion_detected){
      motion_detected = false;
      Serial.println("Motion Ended");
      long elapsed = millis() - motion_start;
      Serial.println(elapsed);
      average_light = 0;
      if(number_of_samples > 0){
        average_light = total_light / number_of_samples;
      }
      
      total_light = 0;
      number_of_samples = 0;
      String r = "api_key="+apiKey2+"&motion="+elapsed+"&light="+average_light;
      post(r);
    }
  }
}

void collectSample(){
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
  
  String httpRequestData = "api_key="+apiKey+"&temperature="+t+"&humidity="+h+"&pressure="+p+"&light="+l;
  post(httpRequestData);
//  delay(time_before_next_sample);
}

void post(String httpRequestData){
  // try to post data until it sends
  int response = 1;
  do{
    response = postData(httpRequestData);
    if(response == -1){
    }else if(response == 1){
      // reconnect to wifi
      connectToWifi();
    }
  }while(response != 200);
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


int postData(String httpRequestData){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(httpRequestData);
//    String response = http.getString();
    http.end();
    Serial.print("Respone Code: ");
    Serial.println(httpResponseCode);
    return httpResponseCode;
  }
  return 1;
}
