#pragma once

#include <PeriodicAction.h>
#include "Display.h"

class Visualization : public PeriodicAction {
public:
  Visualization(Display* display, unsigned long interval);
  virtual void render() = 0;

protected:
  Display* display;
};
