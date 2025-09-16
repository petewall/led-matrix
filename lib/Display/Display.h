#pragma once

#include <stdlib.h>

class Display {
public:
  Display();

  bool setPixel(uint8_t x, uint8_t y, bool on);
  bool getPixel(uint8_t x, uint8_t y);
  uint8_t width() const;
  uint8_t height() const;
  uint32_t rowBits(uint8_t y) const;
  
  bool needsRefresh();
  void refresh();
  void clear();
  void fill(bool on);

private:
  bool dirty;
  uint32_t* frameBuffer;
};
