#include "Text.h"
#include "Font.h"

Text::Text(const char* text, Display* display)
: StaticVisualization(display), text(text) {
  // Draw once for static text
  this->render();
}

void Text::render() {
  if (!text || !*text) {
    return;
  }
  this->display->clear();
  uint8_t x = 0;
  const uint8_t maxWidth = this->display->width();
  const uint8_t maxHeight = this->display->height();
  // Vertically center the 4x6 font
  uint8_t yOffset = 0;
  if (maxHeight > Font4x6::HEIGHT) {
    yOffset = (uint8_t)((maxHeight - Font4x6::HEIGHT) / 2);
  }

  for (const char* p = text; *p; ++p) {
    if (x + Font4x6::WIDTH > maxWidth) {
      break; // stop if no space for the next glyph
    }
    Font4x6::Glyph g = Font4x6::glyphFor(*p);
    for (uint8_t cx = 0; cx < Font4x6::WIDTH; ++cx) {
      uint8_t col = g.cols[cx];
      for (uint8_t ry = 0; ry < Font4x6::HEIGHT; ++ry) {
        bool on = (col >> ry) & 0x01;
        if (on) {
          uint8_t y = (uint8_t)(yOffset + ry);
          if (y < maxHeight) {
            this->display->setPixel(x + cx, y, true);
          }
        }
      }
    }
    x += (Font4x6::WIDTH + Font4x6::SPACING);
    if (x >= maxWidth) {
      break;
    }
  }
}
