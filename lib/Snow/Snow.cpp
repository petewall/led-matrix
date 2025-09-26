#include "Snow.h"

#include <stdlib.h>
#include <vector>

namespace {
constexpr uint8_t MAX_WIND_PERCENT = 100;
}

Snow::Snow(Display* display,
           unsigned long gravity,
           unsigned long snowRate,
           unsigned long meltRate,
           uint8_t wind)
: Visualization(display, TICK_INTERVAL_MS),
  gravity(gravity),
  snowRate(snowRate),
  meltRate(meltRate),
  wind(wind > MAX_WIND_PERCENT ? MAX_WIND_PERCENT : wind),
  gravityAccumulator(0),
  snowAccumulator(0),
  meltAccumulator(0),
  snowRows{}
{
  snowRows.fill(0);
  if (this->display) {
    this->display->clear();
  }
}

bool Snow::handlePixelChange(uint8_t x, uint8_t y, bool on) {
  if (!display) {
    return false;
  }
  if (x >= display->width() || y >= display->height()) {
    return false;
  }
  uint32_t mask = (1UL << x);
  uint32_t& row = snowRows[y];
  bool alreadyOn = (row & mask) != 0;
  bool stateChanged = false;
  if (on) {
    if (!alreadyOn) {
      row |= mask;
      stateChanged = true;
    }
  } else {
    if (alreadyOn) {
      row &= ~mask;
      stateChanged = true;
    }
  }
  if (stateChanged) {
    render();
  }
  return true;
}

unsigned long Snow::getGravity() const {
  return gravity;
}

void Snow::setGravity(unsigned long value) {
  gravity = value;
  if (gravity == 0) {
    gravityAccumulator = 0;
  } else if (gravityAccumulator > gravity) {
    gravityAccumulator = gravityAccumulator % gravity;
  }
}

unsigned long Snow::getSnowRate() const {
  return snowRate;
}

void Snow::setSnowRate(unsigned long value) {
  snowRate = value;
}

unsigned long Snow::getMeltRate() const {
  return meltRate;
}

void Snow::setMeltRate(unsigned long value) {
  meltRate = value;
}

uint8_t Snow::getWind() const {
  return wind;
}

void Snow::setWind(uint8_t value) {
  wind = value > MAX_WIND_PERCENT ? MAX_WIND_PERCENT : value;
}

bool Snow::run() {
  const unsigned long elapsed = TICK_INTERVAL_MS;
  bool changed = false;

  if (snowRate > 0) {
    snowAccumulator += elapsed;
    while (snowAccumulator >= snowRate) {
      if (addSnowflake()) {
        changed = true;
      }
      snowAccumulator -= snowRate;
    }
  }

  if (meltRate > 0) {
    meltAccumulator += elapsed;
    while (meltAccumulator >= meltRate) {
      if (meltSnowflake()) {
        changed = true;
      }
      meltAccumulator -= meltRate;
    }
  }

  if (gravity == 0) {
    // No gravity movement when set to zero.
  } else {
    gravityAccumulator += elapsed;
    while (gravityAccumulator >= gravity) {
      if (applyGravityAndWind()) {
        changed = true;
      }
      gravityAccumulator -= gravity;
    }
  }

  if (changed) {
    render();
  }

  return true;
}

void Snow::render() {
  if (!display) {
    return;
  }
  display->clear();
  for (uint8_t y = 0; y < display->height() && y < LED_MATRIX_ROWS; ++y) {
    uint32_t row = snowRows[y];
    if (!row) {
      continue;
    }
    uint8_t width = display->width();
    for (uint8_t x = 0; x < width && x < LED_MATRIX_COLS; ++x) {
      if ((row >> x) & 0x01U) {
        display->setPixel(x, y, true);
      }
    }
  }
}

bool Snow::addSnowflake() {
  if (!display) {
    return false;
  }
  const uint8_t width = display->width();
  if (width == 0) {
    return false;
  }
  uint32_t filledMask = fullRowMask(width);
  if ((snowRows[0] & filledMask) == filledMask) {
    return false;
  }
  const uint8_t attempts = width;
  for (uint8_t i = 0; i < attempts; ++i) {
    uint8_t x = (uint8_t)(rand() % width);
    uint32_t mask = (1UL << x);
    if ((snowRows[0] & mask) == 0) {
      snowRows[0] |= mask;
      return true;
    }
  }
  for (uint8_t x = 0; x < width; ++x) {
    uint32_t mask = (1UL << x);
    if ((snowRows[0] & mask) == 0) {
      snowRows[0] |= mask;
      return true;
    }
  }
  return false;
}

bool Snow::meltSnowflake() {
  if (!display) {
    return false;
  }
  const uint8_t width = display->width();
  const uint8_t height = display->height();
  if (width == 0 || height == 0) {
    return false;
  }

  std::vector<uint16_t> candidates;
  candidates.reserve(width * height);

  for (uint8_t y = 0; y < height && y < LED_MATRIX_ROWS; ++y) {
    uint32_t row = snowRows[y];
    if (row == 0) {
      continue;
    }
    uint32_t belowRow = (y + 1 < height && (y + 1) < LED_MATRIX_ROWS) ? snowRows[y + 1] : 0;
    for (uint8_t x = 0; x < width && x < LED_MATRIX_COLS; ++x) {
      uint32_t mask = (1UL << x);
      if ((row & mask) == 0) {
        continue;
      }
      bool supported = (y == height - 1) || ((belowRow & mask) != 0);
      if (!supported) {
        continue;
      }
      bool exposedAbove = (y == 0) || ((snowRows[y - 1] & mask) == 0);
      if (!exposedAbove) {
        continue;
      }
      candidates.push_back((uint16_t)((y << 8) | x));
    }
  }

  if (candidates.empty()) {
    // Fallback: remove any snow pixel starting from the highest row
    for (uint8_t y = 0; y < height && y < LED_MATRIX_ROWS; ++y) {
      uint32_t row = snowRows[y];
      if (!row) {
        continue;
      }
      for (uint8_t x = 0; x < width && x < LED_MATRIX_COLS; ++x) {
        uint32_t mask = (1UL << x);
        if (row & mask) {
          snowRows[y] &= ~mask;
          return true;
        }
      }
    }
    return false;
  }

  uint16_t choice = candidates[(size_t)(rand() % candidates.size())];
  uint8_t y = (uint8_t)(choice >> 8);
  uint8_t x = (uint8_t)(choice & 0xFF);
  snowRows[y] &= ~(1UL << x);
  return true;
}

bool Snow::applyGravityAndWind() {
  std::array<uint32_t, LED_MATRIX_ROWS> nextRows{};
  nextRows.fill(0);
  bool changed = false;

  const uint8_t width = display ? display->width() : LED_MATRIX_COLS;
  const uint8_t height = display ? display->height() : LED_MATRIX_ROWS;

  for (int y = height - 1; y >= 0; --y) {
    uint32_t row = snowRows[(size_t)y];
    if (row == 0) {
      continue;
    }
    for (uint8_t x = 0; x < width && x < LED_MATRIX_COLS; ++x) {
      uint32_t mask = (1UL << x);
      if ((row & mask) == 0) {
        continue;
      }

      uint8_t destX = x;
      uint8_t destY = (uint8_t)y;

      auto occupied = [&](uint8_t checkX, uint8_t checkY) -> bool {
        uint32_t bit = (1UL << checkX);
        return ((snowRows[(size_t)checkY] | nextRows[(size_t)checkY]) & bit) != 0;
      };

      if (y < height - 1) {
        uint8_t nextY = (uint8_t)(y + 1);
        if (!occupied(destX, nextY)) {
          destY = nextY;
          if (wind > 0) {
            uint8_t roll = (uint8_t)(rand() % 100);
            if (roll < wind) {
              bool moveRight = (rand() & 1) != 0;
              int candidateX = (int)destX + (moveRight ? 1 : -1);
              if (candidateX >= 0 && candidateX < width) {
                uint8_t cx = (uint8_t)candidateX;
                if (!occupied(cx, nextY)) {
                  destX = cx;
                }
              }
            }
          }
        } else if (wind > 0) {
          uint8_t roll = (uint8_t)(rand() % 100);
          if (roll < wind) {
            bool moveRight = (rand() & 1) != 0;
            int candidateX = (int)x + (moveRight ? 1 : -1);
            if (candidateX >= 0 && candidateX < width) {
              uint8_t cx = (uint8_t)candidateX;
              if (!occupied(cx, nextY)) {
                destX = cx;
                destY = nextY;
              }
            }
          }
        }
      }

      if (destX != x || destY != y) {
        changed = true;
      }
      nextRows[(size_t)destY] |= (1UL << destX);
    }
  }

  snowRows = nextRows;
  return changed;
}

uint32_t Snow::fullRowMask(uint8_t width) const {
  if (width >= 32) {
    return 0xFFFFFFFFUL;
  }
  uint32_t mask = 0;
  for (uint8_t x = 0; x < width; ++x) {
    mask |= (1UL << x);
  }
  return mask;
}
