#include "Display.h"
#include "hardware.h"

Display::Display()
: dirty(false)
{
    frameBuffer = new uint32_t[LED_MATRIX_ROWS];
}

bool Display::setPixel(uint8_t x, uint8_t y, bool on) {
  bool changed = false;
  if (x < LED_MATRIX_COLS && y < LED_MATRIX_ROWS) {
    changed = this->getPixel(x, y) != on;
    this->dirty = this->dirty | changed;
    uint32_t m = (1UL << x);
    if (on) {
      frameBuffer[y] |= m;
    }  else {
      frameBuffer[y] &= ~m;
    }
  }
  return changed;
}

bool Display::getPixel(uint8_t x, uint8_t y) {
  if (x < LED_MATRIX_COLS && y < LED_MATRIX_ROWS) {
    return (frameBuffer[y] >> x) & 1U;
  }
  return false;
}

uint8_t Display::width() const {
  return LED_MATRIX_COLS;
}

uint8_t Display::height() const {
  return LED_MATRIX_ROWS;
}

uint32_t Display::rowBits(uint8_t y) const {
  if (y < LED_MATRIX_ROWS) {
    return frameBuffer[y];
  }
  return 0;
}

bool Display::needsRefresh() {
  return this->dirty;
}

void Display::refresh() {
  this->dirty = false;
}

void Display::clear() {
  for (uint8_t y = 0; y < LED_MATRIX_ROWS; y++) {
    frameBuffer[y] = 0;
  }
  this->dirty = true;
}

void Display::fill(bool on) {
  uint32_t mask = 0;
  if (on) {
    if (LED_MATRIX_COLS >= 32) {
      mask = 0xFFFFFFFFUL;
    } else {
      for (uint8_t i = 0; i < LED_MATRIX_COLS; i++) {
        mask |= (1UL << i);
      }
    }
  }
  for (uint8_t y = 0; y < LED_MATRIX_ROWS; y++) {
    frameBuffer[y] = mask;
  }
  this->dirty = true;
}
