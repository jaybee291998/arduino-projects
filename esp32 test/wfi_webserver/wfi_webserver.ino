#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#include <ArduinoJson.h>

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 4

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600        /* Time ESP32 will go to sleep (in seconds) */

StaticJsonDocument<250> jsonDocument;

byte max_tries = 20;

float h;
float t;

String apiKey = "F3B5SDC8JXTYMBVH";

const char* ssid = "wifi-sa-bahay-V2.4";
const char* password = "wifi-sa-bahay-V2.4-password";

WebServer server(80);


const char* serverName = "http://api.thingspeak.com/update";
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(dht_dpin, DHTTYPE); 
void setup(void)
{ 
  dht.begin();
  lcd.begin();
  Serial.begin(115200);
  connectToWifi();
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
//  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//  esp_deep_sleep_start();
}
void loop() {
  server.handleClient();
  readDHT();  
  delay(1000);  
}

void readDHT(){
  do{
    h = dht.readHumidity();
    t = dht.readTemperature();
  }while(isnan(h) || isnan(t));
  float heat_index = dht.computeHeatIndex(t, h, false);

  lcd.clear();
  lcd.print("T:");
  lcd.print(t);
  lcd.print(", H:");
  lcd.print(h);
  lcd.setCursor(0,1);
  lcd.print("HIC: ");
  lcd.print(heat_index);
  delay(500);  
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
  lcd.clear();
  lcd.print("sucess");
}

void connectToWifi(){
  Serial.println("Connecting");
  lcd.clear();
  lcd.print("Connecting");
  lcd.setCursor(0,1);
  byte connection_tries = 0;
//  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    lcd.print(".");
    delay(500);
    if(connection_tries >= max_tries){
      // if it fails to connect after 20 tries, sleep for 1 second
      esp_sleep_enable_timer_wakeup(1 * uS_TO_S_FACTOR);
      esp_deep_sleep_start();
    }
    connection_tries++;
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}


int postData(float temp, float humid, float hic){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "api_key="+apiKey+"&field1="+temp+"&field2="+humid+"&field3="+hic;
    Serial.print("Payload: ");
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("Response: ");
    Serial.println(httpResponseCode);
    http.end();
    return httpResponseCode;
  }
  return 1;
}

// Serving Hello world
void acceptRFIDPost() {
  if (server.hasArg("rfid")) {
    String rfid = server.arg("rfid");
    Serial.print("RFID: ");
    Serial.println(rfid);
    DynamicJsonDocument doc(512);
    doc["status"] = "From ESP32:OK";
    doc["data"] = rfid;
    String buf;
    serializeJson(doc, buf);
    server.send(201, F("application/json"), buf);
    lcd.clear();
    lcd.print("Writing");
    lcd.setCursor(0,1);
    lcd.print(rfid);
    delay(1000);
  }else{
      DynamicJsonDocument doc(512);
      doc["status"] = "From ESP32:Invalid Post Parameters";
   
      String buf;
      serializeJson(doc, buf);
      server.send(422, F("application/json"), buf);
    
  }
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
