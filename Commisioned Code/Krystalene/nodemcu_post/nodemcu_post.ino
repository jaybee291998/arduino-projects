/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  
  Code compatible with ESP8266 Boards Version 3.0.0 or above 
  (see in Tools > Boards > Boards Manager > ESP8266)
*/
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "neeco";
const char* password = "123456789";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.137.1/arduino/receive.php";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while(!Serial) continue;

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      StaticJsonDocument<1000> doc;
      auto error = deserializeJson(doc, Serial);
      digitalWrite(LED_BUILTIN, HIGH);
      if(error){
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
        return;
      }else{
        WiFiClient client;
        HTTPClient http;
        // Your Domain name with URL path or IP address with path
        http.begin(client, serverName);
  
        // Specify content-type header
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // Data to send with HTTP POST
        String code = doc["rfid_code"];
        int temperature = doc["temperature"];
        String httpRequestData = "rfid_code="+code+"&temperature="+temperature;           
        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.print(httpRequestData);
        
        // Free resources
        http.end();
      }
      digitalWrite(LED_BUILTIN, LOW);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void receiveFromArduino(){
   String IncomingString="";
   boolean StringReady = false;
   
   while (Serial.available()){
     IncomingString=Serial.readString();
     StringReady= true;
    }

    if (StringReady){
       Serial.println("(ESP)Received String: " + IncomingString); 
    }
}
