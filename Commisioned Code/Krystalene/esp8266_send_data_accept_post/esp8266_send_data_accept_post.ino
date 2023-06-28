/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  
  Code compatible with ESP8266 Boards Version 3.0.0 or above 
  (see in Tools > Boards > Boards Manager > ESP8266)
*/
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "neeco";
const char* password = "123456789";

ESP8266WebServer server(80);

SoftwareSerial nodemcu(D6, D5);

//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.137.1/arduino/receive.php";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
void setup() {
  Serial.begin(9600);
  nodemcu.begin(9600);

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
//   Activate mDNS this is used to be able to connect to the server
//   with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    
    if(WiFi.status()== WL_CONNECTED){
      StaticJsonDocument<1000> doc;
      auto error = deserializeJson(doc, nodemcu);
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
        double temperature = doc["temperature"];
        String httpRequestData = "rfid_code="+code+"&temperature="+temperature;           
        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.print(httpRequestData);
        
        // Free resources
        http.end();
      }
    }
    else {
//      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

// Serving Hello world
void acceptRFIDPost() {
    String rf = server.arg("plain");
    String rfid = rf.substring(5);
    Serial.print("RFID: ");
    Serial.println(rfid);
    DynamicJsonDocument doc(512);
    doc["status"] = "From ESP8266:OK";
 
    String buf;
    serializeJson(doc, buf);
    server.send(201, F("application/json"), buf);
    nodemcu.print(rfid);
}
// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Welcome to the REST Web Server"));
    });
    // handle post request
    server.on(F("/writeRFID"), HTTP_POST, acceptRFIDPost);
}
 
// Manage not found URL
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
