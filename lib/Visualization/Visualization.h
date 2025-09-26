#pragma once

#include <stdint.h>
#include <PeriodicAction.h>
#include "Display.h"

class Visualization : public PeriodicAction {
public:
  Visualization(Display* display, unsigned long interval);
  virtual ~Visualization() = default;

  virtual bool handlePixelChange(uint8_t x, uint8_t y, bool on);

protected:
  virtual void render() = 0;

  Display* display;
};
