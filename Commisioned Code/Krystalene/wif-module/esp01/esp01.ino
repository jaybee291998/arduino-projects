#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "JAYVEE29 1097";
const char* password = "m194T3-0";

const char* serverName = "192.168.137.1/arduino/receive.php";

unsigned long lastTime = 0;

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
  if(WiFi.status() == WL_CONNECTED){
    StaticJsonDocument<1000> doc;
    auto error = deserializeJson(doc, Serial);
    if(error){
      Serial.print("deserializeJson failed with code ");
      Serial.println(error.c_str());
      return;
    }else{
      digitalWrite(LED_BUILTIN, HIGH);
      WiFiClient client;
      HTTPClient http;

      http.begin(client, serverName);

      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      String rfid_code = doc["rfid_code"];
      double temperature = doc["temperature"];

      String httpRequestData = "rfid_code="+rfid_code+"&temperature="+temperature;

      int httpResponseCode = http.POST(httpRequestData);
      Serial.print(httpResponseCode);

      http.end();
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
