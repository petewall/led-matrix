#pragma once

#include "Display.h"
#include "Visualization.h"

class StaticVisualization : public Visualization {
public:
  StaticVisualization(Display* display);

protected:
  virtual bool run() override;
};
