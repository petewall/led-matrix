#include "Text.h"
#include "Font.h"

Text::Text(const char* text, Display* display)
: StaticVisualization(display), text(text) {}

void Text::render() {
  if (!text || !*text) {
    return;
  }
  // Render static text starting at (0,0) using the 3x5 font
  this->display->clear();
  uint8_t x = 0;
  const uint8_t maxWidth = this->display->width();
  const uint8_t maxHeight = this->display->height();
  // Vertically center the 4x6 font inside the display height
  uint8_t yOffset = 0;
  if (maxHeight > Font3x5::HEIGHT) {
    yOffset = (uint8_t)((maxHeight - Font3x5::HEIGHT) / 2);
  }

  for (const char* p = text; *p; ++p) {
    if (x + Font3x5::WIDTH > maxWidth) {
      break; // stop if no space for the next glyph
    }
    Font3x5::Glyph g = Font3x5::glyphFor(*p);
    for (uint8_t cx = 0; cx < Font3x5::WIDTH; ++cx) {
      uint8_t col = g.cols[cx];
      for (uint8_t ry = 0; ry < Font3x5::HEIGHT; ++ry) {
        bool on = (col >> ry) & 0x01;
        if (on) {
          uint8_t y = (uint8_t)(yOffset + ry);
          if (y < maxHeight) {
            this->display->setPixel(x + cx, y, true);
          }
        }
      }
    }
    x += (Font3x5::WIDTH + Font3x5::SPACING);
    if (x >= maxWidth) {
      break;
    }
  }
}
