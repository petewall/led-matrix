#include "Visualization.h"

Visualization::Visualization(Display* display, unsigned long interval)
: PeriodicAction(interval), display(display) {}
