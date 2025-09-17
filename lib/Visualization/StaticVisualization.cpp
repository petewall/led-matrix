#include "StaticVisualization.h"

StaticVisualization::StaticVisualization(Display* display)
: Visualization(display, UINT32_MAX) {}

bool StaticVisualization::run() {
    this->render();
    return true;
}
