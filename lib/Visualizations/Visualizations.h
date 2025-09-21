#pragma once

#include <stddef.h>

class Display;
class Visualization;

struct VisualizationDefinition {
  const char* id;
  const char* label;
  Visualization* (*create)(Display* display);
};

const VisualizationDefinition* availableVisualizations(size_t* count);
const VisualizationDefinition* findVisualization(const char* id);
const VisualizationDefinition* defaultVisualization();
Visualization* createVisualization(const VisualizationDefinition* definition, Display* display);
Visualization* createVisualizationById(const char* id, Display* display);
