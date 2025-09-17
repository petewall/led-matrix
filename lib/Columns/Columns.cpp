#include "Columns.h"
#include "hardware.h"

Columns::Columns(Display* display, unsigned long speed, bool bounce)
: Visualization(display, speed), bounce(bounce), movingRight(true), currentColumn(0)
{}

bool Columns::run() {
  render();
  if (bounce) {
    if (movingRight) {
      ++currentColumn;
      if (currentColumn == LED_MATRIX_COLS - 1) {
        movingRight = false;
      }
    } else {
      --currentColumn;
      if (currentColumn == 0) {
        movingRight = true;
      }

    }
  } else {
    currentColumn = (currentColumn + 1) % LED_MATRIX_COLS;
  }
  return true;
}

void Columns::render() {
  for (uint8_t x = 0; x < LED_MATRIX_COLS; ++x) {
    for (uint8_t y = 0; y < LED_MATRIX_ROWS; ++y) {
      display->setPixel(x, y, x == currentColumn);
    }
  }
}
