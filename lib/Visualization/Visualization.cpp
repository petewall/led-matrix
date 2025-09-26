#include "Visualization.h"

Visualization::Visualization(Display* display, unsigned long interval)
: PeriodicAction(interval), display(display) {}

bool Visualization::handlePixelChange(uint8_t, uint8_t, bool) {
  return false;
}
