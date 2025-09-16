#pragma once

#include <string>
#include "Visualization.h"

class Columns : public Visualization {
public:
  Columns(Display* display);
  
protected:
  bool run() override;
  void render() override;

private:
  uint8_t currentColumn;
};
