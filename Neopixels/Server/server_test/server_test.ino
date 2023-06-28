#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <FreeRTOS.h>

const char *SSID = "wifi-sa-bahay-V2.4";
const char *PWD = "wifi-sa-bahay-V2.4-password";

WebServer server(80);

StaticJsonDocument<250> jsonDocument;
char buffer[250];

void setup_routing(){
  server.on("/post", HTTP_POST, handle_post);

  server.begin();
}

void handle_post(){
  String message = server.arg("message");
  int foregroundColor = server.arg("foreground-color").toInt();
  int backgroundColor = server.arg("background-color").toInt();
  int cycles = server.arg("cycles").toInt();
  int brightness = server.arg("brightness").toInt();
  Serial.print("Message: ");
  Serial.println(message);
  Serial.print("Foreground: ");
  Serial.println(foregroundColor);
  Serial.print("Background: ");
  Serial.println(backgroundColor);
  Serial.print("Cycles: ");
  Serial.println(cycles);
  Serial.print("Brightness: ");
  Serial.println(brightness);
  server.send(200, "application/json", "{OK}");
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.print("Connecting to WiFi");
  WiFi.begin(SSID, PWD);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
  setup_routing();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
