#pragma once

#include <PeriodicAction.h>
#include "Display.h"

class Visualization : public PeriodicAction {
public:
  Visualization(Display* display, unsigned long interval);

protected:
  virtual void render() = 0;

  Display* display;
};
