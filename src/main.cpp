// System dependencies
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <ESPmDNS.h>
#endif

// External libraries

// Internal libraries
#include "hardware.h"
#include "LedMatrix.h"
#include "Display.h"
#include "Columns.h"
#include "Text.h"
#include "Visualization.h"

#include "WebServer.h"


LedMatrix* ledMatrix;
Display* display;
Visualization* currentVisualization;

WebServer* webServer;

void connectToWiFi(void) {
  WiFi.mode(WIFI_STA);
  #ifdef HOSTNAME
    #if defined(ESP8266)
      WiFi.hostname(HOSTNAME);
    #elif defined(ESP32)
      WiFi.setHostname(HOSTNAME);
    #endif
  #endif
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

  #if (defined(ESP8266) || defined(ESP32)) && defined(HOSTNAME)
    if (!MDNS.begin(HOSTNAME)) {
      Serial.println("Error setting up mDNS responder");
    } else {
      Serial.print("mDNS responder started: ");
      Serial.print(HOSTNAME);
      Serial.println(".local");
      MDNS.addService("http", "tcp", 80);
    }
  #endif
}

void setup() {
  Serial.begin(115200);

  ledMatrix = new LedMatrix();
  display = new Display();
  currentVisualization = new Text("I luv u!", display);
  // currentVisualization = new Columns(display, 50, true);

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
  #if defined(ESP8266) && defined(HOSTNAME)
    MDNS.update();
  #endif
}
