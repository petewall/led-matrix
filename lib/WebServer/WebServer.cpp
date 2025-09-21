#include "WebServer.h"
#include "hardware.h"
#include "LedMatrix.h"

#include <ESPAsyncWebServer.h>
#if defined(ESP8266)
#include <LittleFS.h>
#elif defined(ESP32)
#include <LittleFS.h>
#endif

WebServer::WebServer(Display* display,
                     LedMatrix* ledMatrix,
                     const VisualizationDefinition* visualizationDefinitions,
                     size_t visualizationDefinitionCount,
                     VisualizationSetter setVisualizationCallback,
                     VisualizationGetter getCurrentVisualizationIdCallback)
  : display(display),
    ledMatrix(ledMatrix),
    asyncWebServer(nullptr),
    visualizationDefinitions(visualizationDefinitions),
    visualizationDefinitionCount(visualizationDefinitionCount),
    setVisualizationCallback(setVisualizationCallback),
    getCurrentVisualizationIdCallback(getCurrentVisualizationIdCallback)
{
  asyncWebServer = new AsyncWebServer(80);
  // Mount LittleFS and serve static files from it (default to index.html)
#if defined(ESP8266)
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
  } else {
    Serial.println("LittleFS mounted");
  }
#elif defined(ESP32)
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
  } else {
    Serial.println("LittleFS mounted");
  }
#endif

  asyncWebServer->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  asyncWebServer->on("/hardware.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{";
    response += "\"columns\":";
    response += LED_MATRIX_COLS;
    response += ",\"rows\":";
    response += LED_MATRIX_ROWS;
    response += "}";
    request->send(200, "application/json", response);
  });

  // Return framebuffer as an array of row bitmasks
  asyncWebServer->on("/display", HTTP_GET, [this](AsyncWebServerRequest *request) {
    String response = "{";
    response += "\"columns\":";
    response += this->display->width();
    response += ",\"rows\":";
    response += this->display->height();
    response += ",\"framebuffer\":[";
    for (uint8_t y = 0; y < this->display->height(); y++) {
      if (y > 0) response += ",";
      response += (unsigned long)this->display->rowBits(y);
    }
    response += "]}";
    request->send(200, "application/json", response);
  });

  // Set a pixel: PUT /display?x=..&y=..&on=0|1 (also accepts body params)
  asyncWebServer->on("/display", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    auto getParam = [&](const char* name) -> const AsyncWebParameter* {
      if (request->hasParam(name)) {
        return request->getParam(name);
      }
      if (request->hasParam(name, true)) {
        return request->getParam(name, true);
      }
      return nullptr;
    };

    const AsyncWebParameter* px = getParam("x");
    const AsyncWebParameter* py = getParam("y");
    const AsyncWebParameter* pon = getParam("on");
    if (!px || !py || !pon) {
      request->send(400, "application/json", "{\"error\":\"x, y, and on are required\"}");
      return;
    }

    int x = px->value().toInt();
    int y = py->value().toInt();
    String onStr = pon->value();
    onStr.toLowerCase();
    bool on = (onStr == "1" || onStr == "true" || onStr == "on");

    if (x < 0 || y < 0 || x >= this->display->width() || y >= this->display->height()) {
      request->send(400, "application/json", "{\"error\":\"x or y out of range\"}");
      return;
    }

    bool changed = this->display->setPixel((uint8_t)x, (uint8_t)y, on);
    String json = "{";
    json += "\"x\":"; json += x; json += ",";
    json += "\"y\":"; json += y; json += ",";
    json += "\"on\":"; json += (on ? "true" : "false"); json += ",";
    json += "\"changed\":"; json += (changed ? "true" : "false");
    json += "}";
    request->send(200, "application/json", json);
  });

  // Fill all pixels on/off: POST /display/fill?on=0|1 (default 1)
  asyncWebServer->on("/display/fill", HTTP_POST, [this](AsyncWebServerRequest *request) {
    bool on = true;
    if (request->hasParam("on")) {
      String v = request->getParam("on")->value();
      v.toLowerCase();
      on = (v == "1" || v == "true" || v == "on");
    } else if (request->hasParam("on", true)) {
      String v = request->getParam("on", true)->value();
      v.toLowerCase();
      on = (v == "1" || v == "true" || v == "on");
    }
    this->display->fill(on);
    request->send(200, "application/json", String("{\"filled\":" ) + (on ? "true" : "false") + "}");
  });

  // Clear all pixels: DELETE /display
  asyncWebServer->on("/display", HTTP_DELETE, [this](AsyncWebServerRequest *request) {
    this->display->clear();
    request->send(200, "application/json", "{\"cleared\":true}");
  });

  asyncWebServer->on("/visualizations", HTTP_GET, [this](AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"current\":";
    json += "\"";
    if (this->getCurrentVisualizationIdCallback) {
      json += this->getCurrentVisualizationIdCallback();
    }
    json += "\",";
    json += "\"visualizations\":[";
    for (size_t i = 0; i < this->visualizationDefinitionCount; i++) {
      if (i > 0) {
        json += ",";
      }
      json += "{";
      json += "\"id\":\"";
      json += this->visualizationDefinitions[i].id;
      json += "\",\"label\":\"";
      json += this->visualizationDefinitions[i].label;
      json += "\"}";
    }
    json += "]}";
    request->send(200, "application/json", json);
  });

  asyncWebServer->on("/visualizations", HTTP_POST, [this](AsyncWebServerRequest *request) {
    const AsyncWebParameter* pid = nullptr;
    if (request->hasParam("id")) {
      pid = request->getParam("id");
    } else if (request->hasParam("id", true)) {
      pid = request->getParam("id", true);
    }

    if (!pid) {
      request->send(400, "application/json", "{\"error\":\"id is required\"}");
      return;
    }

    String id = pid->value();
    bool success = false;
    if (this->setVisualizationCallback) {
      success = this->setVisualizationCallback(id.c_str());
    }

    if (!success) {
      request->send(404, "application/json", "{\"error\":\"visualization not found\"}");
      return;
    }

    String json = "{";
    json += "\"current\":\"";
    if (this->getCurrentVisualizationIdCallback) {
      json += this->getCurrentVisualizationIdCallback();
    }
    json += "\"}";
    request->send(200, "application/json", json);
  });

  // Brightness endpoints
  // GET /brightness -> {"brightness":0..15}
  asyncWebServer->on("/brightness", HTTP_GET, [this](AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"brightness\":"; json += (int)this->ledMatrix->intensity();
    json += "}";
    request->send(200, "application/json", json);
  });
  // PUT /brightness?value=0..15 (also accepts body param)
  asyncWebServer->on("/brightness", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    auto getParam = [&](const char* name) -> const AsyncWebParameter* {
      if (request->hasParam(name)) {return request->getParam(name);}
      if (request->hasParam(name, true)) {return request->getParam(name, true);}
      return nullptr;
    };
    const AsyncWebParameter* pv = getParam("value");
    if (!pv) {
      request->send(400, "application/json", "{\"error\":\"value is required\"}");
      return;
    }
    int v = pv->value().toInt();
    if (v < LED_MATRIX_BRIGHTNESS_MIN) {
      v = LED_MATRIX_BRIGHTNESS_MIN;
    } else if (v > LED_MATRIX_BRIGHTNESS_MAX) {
      v = LED_MATRIX_BRIGHTNESS_MAX;
    }
    this->ledMatrix->setIntensity((uint8_t)v);
    String json = "{";
    json += "\"brightness\":"; json += v; json += "}";
    request->send(200, "application/json", json);
  });

  asyncWebServer->begin();
  Serial.println("HTTP server started");
}
