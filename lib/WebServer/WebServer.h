#pragma once

#include "Display.h"
#include "Visualizations.h"

#include <stddef.h>

class LedMatrix; // forward declaration

class AsyncWebServer; // forward declaration

class WebServer {
public:
    using VisualizationSetter = bool (*)(const char* id);
    using VisualizationGetter = const char* (*)();

    WebServer(Display* display,
              LedMatrix* ledMatrix,
              const VisualizationDefinition* visualizationDefinitions,
              size_t visualizationDefinitionCount,
              VisualizationSetter setVisualizationCallback,
              VisualizationGetter getCurrentVisualizationIdCallback);
private:
    Display* display;
    LedMatrix* ledMatrix;
    AsyncWebServer* asyncWebServer;
    const VisualizationDefinition* visualizationDefinitions;
    size_t visualizationDefinitionCount;
    VisualizationSetter setVisualizationCallback;
    VisualizationGetter getCurrentVisualizationIdCallback;
};
