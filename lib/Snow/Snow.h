#pragma once

#include <array>
#include <stdint.h>

#include "Visualization.h"
#include "hardware.h"

class Snow : public Visualization {
public:
  static constexpr unsigned long TICK_INTERVAL_MS = 10;
  static constexpr unsigned long DEFAULT_GRAVITY_MS = 50;
  static constexpr unsigned long DEFAULT_SNOW_RATE_MS = 250;
  static constexpr unsigned long DEFAULT_MELT_RATE_MS = 4000;
  static constexpr uint8_t DEFAULT_WIND_PERCENT = 25;

  Snow(Display* display,
       unsigned long gravity = DEFAULT_GRAVITY_MS,
       unsigned long snowRate = DEFAULT_SNOW_RATE_MS,
       unsigned long meltRate = DEFAULT_MELT_RATE_MS,
       uint8_t wind = DEFAULT_WIND_PERCENT);

  bool handlePixelChange(uint8_t x, uint8_t y, bool on) override;

  unsigned long getGravity() const;
  void setGravity(unsigned long value);

  unsigned long getSnowRate() const;
  void setSnowRate(unsigned long value);

  unsigned long getMeltRate() const;
  void setMeltRate(unsigned long value);

  uint8_t getWind() const;
  void setWind(uint8_t value);

protected:
  bool run() override;
  void render() override;

private:
  bool addSnowflake();
  bool meltSnowflake();
  bool applyGravityAndWind();

  uint32_t fullRowMask(uint8_t width) const;

  unsigned long gravity;
  unsigned long snowRate;
  unsigned long meltRate;
  uint8_t wind;

  unsigned long gravityAccumulator;
  unsigned long snowAccumulator;
  unsigned long meltAccumulator;

  std::array<uint32_t, LED_MATRIX_ROWS> snowRows;
};
