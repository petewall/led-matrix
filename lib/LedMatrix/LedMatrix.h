#pragma once

#include <stdlib.h>
#include "Display.h"

class LedMatrix {
public:
  LedMatrix();

  void set(Display* display);
  void setIntensity(uint8_t value);
  uint8_t intensity() const;
private:
  uint8_t currentIntensity;
};
