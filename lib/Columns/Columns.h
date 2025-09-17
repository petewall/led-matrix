#pragma once

#include <string>
#include "Visualization.h"

class Columns : public Visualization {
public:
  Columns(Display* display, unsigned long speed = 50, bool bounce = false);
  
protected:
  bool run() override;
  void render() override;

private:
  bool bounce;
  bool movingRight;
  uint8_t currentColumn;
};
