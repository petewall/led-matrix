// System dependencies
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

// External libraries

// Internal libraries
#include "hardware.h"
#include "LedMatrix.h"
#include "Display.h"
// #include "Columns.h"
// #include "Off.h"
#include "Visualization.h"

#include "WebServer.h"

LedMatrix* ledMatrix;
Display* display;
Visualization* currentVisualization;

WebServer* webServer;

void connectToWiFi(void) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  ledMatrix = new LedMatrix();
  display = new Display();
  currentVisualization = NULL;

  connectToWiFi();
  webServer = new WebServer(display, ledMatrix);
}

void loop() {
  unsigned long now = millis();
  if (currentVisualization != NULL) {
    currentVisualization->check(now);
  }
  if (display->needsRefresh()) {
    ledMatrix->set(display);
  }
}
