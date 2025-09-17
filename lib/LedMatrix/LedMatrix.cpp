#include "hardware.h"
#include "LedMatrix.h"
#include <LedControl.h>

#define DEFAULT_BRIGHTNESS 1

LedControl lc(PIN_DIN, PIN_CLK, PIN_CS, NUM_DEVICES);

LedMatrix::LedMatrix()
: currentIntensity(DEFAULT_BRIGHTNESS)
{
  for (uint8_t i = 0; i < NUM_DEVICES; ++i) {
    lc.shutdown(i, false);
    lc.setIntensity(i, currentIntensity);
    lc.clearDisplay(i);
  }
}

void LedMatrix::set(Display* display) {
  for (uint8_t x = 0; x < LED_MATRIX_COLS; ++x) {
    for (uint8_t y = 0; y < LED_MATRIX_ROWS; ++y) {
      lc.setLed(x / 8, x % 8, y, display->getPixel(x, y));
    }
  }
}

void LedMatrix::setIntensity(uint8_t value) {
  if (value > LED_MATRIX_BRIGHTNESS_MAX) {
    value = LED_MATRIX_BRIGHTNESS_MAX;
  }
  currentIntensity = value;
  for (uint8_t i = 0; i < NUM_DEVICES; ++i) {
    lc.setIntensity(i, currentIntensity);
  }
}

uint8_t LedMatrix::intensity() const {
  return currentIntensity;
}
