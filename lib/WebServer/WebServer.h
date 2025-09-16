#pragma once

#include "Display.h"

class LedMatrix; // forward declaration

class AsyncWebServer; // forward declaration

class WebServer {
public:
    WebServer(Display* display, LedMatrix* ledMatrix);
private:
    Display* display;
    LedMatrix* ledMatrix;
    AsyncWebServer* asyncWebServer;
};
