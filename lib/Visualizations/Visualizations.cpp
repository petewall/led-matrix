#include "Visualizations.h"

#include <string.h>

#include "Clock.h"
#include "Columns.h"
#include "Text.h"

namespace {

Visualization* createClock(Display* display) {
  return new Clock(display);
}

Visualization* createColumns(Display* display) {
  return new Columns(display, 50, true);
}

Visualization* createText(Display* display) {
  return new Text("HELLO", display);
}

constexpr VisualizationDefinition VISUALIZATION_DEFINITIONS[] = {
  {"clock", "Clock", createClock},
  {"columns", "Columns", createColumns},
  {"text", "Text", createText},
};

constexpr size_t VISUALIZATION_COUNT = sizeof(VISUALIZATION_DEFINITIONS) / sizeof(VISUALIZATION_DEFINITIONS[0]);

const VisualizationDefinition* findVisualizationInternal(const char* id) {
  if (!id) {
    return nullptr;
  }
  for (size_t i = 0; i < VISUALIZATION_COUNT; i++) {
    if (strcmp(VISUALIZATION_DEFINITIONS[i].id, id) == 0) {
      return &VISUALIZATION_DEFINITIONS[i];
    }
  }
  return nullptr;
}

}  // namespace

const VisualizationDefinition* availableVisualizations(size_t* count) {
  if (count) {
    *count = VISUALIZATION_COUNT;
  }
  return VISUALIZATION_DEFINITIONS;
}

const VisualizationDefinition* findVisualization(const char* id) {
  return findVisualizationInternal(id);
}

const VisualizationDefinition* defaultVisualization() {
  return &VISUALIZATION_DEFINITIONS[0];
}

Visualization* createVisualization(const VisualizationDefinition* definition, Display* display) {
  if (!definition || !definition->create) {
    return nullptr;
  }
  return definition->create(display);
}

Visualization* createVisualizationById(const char* id, Display* display) {
  const VisualizationDefinition* definition = findVisualizationInternal(id);
  if (!definition) {
    return nullptr;
  }
  return createVisualization(definition, display);
}

