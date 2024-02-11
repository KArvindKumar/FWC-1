#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


// Replace with your network credentials
#ifndef STASSID
#define STASSID "Mr_Stance_39"
#define STAPSK  "1234567890"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

void OTAsetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.begin();
}

void OTAloop() {
  ArduinoOTA.handle();
}

void setup() {
  OTAsetup();
  
  // Custom setup code from code-1
  pinMode(2, OUTPUT);
}

void loop() {
  OTAloop();
  delay(10);  // If no custom loop code, ensure to have a delay in loop

  // Custom loop code from code-1
  int X = 1;
  int Y = 0;
  int Z = 1;
  int W = 0;
  int R1 = (X && !Y && Z && W) || (X && Y && Z && !W);
  int R2 = (X && Y);
  if (R1 == R2) {
    digitalWrite(2, HIGH);
    delay(1000);
  } else {
    digitalWrite(2, LOW);
    delay(1000);
  }
}
