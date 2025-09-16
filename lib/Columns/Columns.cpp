#include "Columns.h"
#include "hardware.h"

Columns::Columns(Display* display)
: Visualization(display, 500), currentColumn(0)
{}

bool Columns::run() {
  this->render();
  this->currentColumn = (this->currentColumn + 1) % LED_MATRIX_COLS;
  return true;
}

void Columns::render() {
  for (uint8_t x = 0; x < LED_MATRIX_COLS; ++x) {
    for (uint8_t y = 0; y < LED_MATRIX_ROWS; ++y) {
      display->setPixel(x, y, x == currentColumn);
    }
  }
}
