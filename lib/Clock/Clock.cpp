#include "Clock.h"
#include "hardware.h"
#include "Font.h"

#include <time.h>
#include <string.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <coredecls.h> // settimeofday_cb in ESP8266 core
#elif defined(ESP32)
#include <WiFi.h>
#endif

// NTP server to use
static const char* NTP_SERVER = "pool.ntp.org";

Clock::Clock(Display* display)
  : Visualization(display, 500), colonOn(true), timeInitialized(false) {
  initTimeOnce();
}

bool Clock::run() {
  // Toggle colon each tick (500ms)
  colonOn = !colonOn;
  render();
  return true;
}

void Clock::initTimeOnce() {
  if (timeInitialized) return;

  // Configure timezone if provided; default to US Central.
  // ESP8266/ESP32 honor POSIX TZ via setenv/tzset.
  #if defined(ESP8266) || defined(ESP32)
    #ifdef TIMEZONE_TZ
      setenv("TZ", TIMEZONE_TZ, 1);
    #else
      setenv("TZ", "CST6CDT,M3.2.0/2,M11.1.0/2", 1);
    #endif
    tzset();
  #endif

  // Configure SNTP time. On ESP32 there's configTzTime; on ESP8266 use configTime.
  #if defined(ESP32)
    #ifdef TIMEZONE_TZ
      configTzTime(TIMEZONE_TZ, NTP_SERVER);
    #else
      // No compile-time TZ provided; TZ env above handles local time.
      configTime(0, 0, NTP_SERVER);
    #endif
  #elif defined(ESP8266)
    configTime(0, 0, NTP_SERVER);
  #endif

  timeInitialized = true;
}

void Clock::render() {
  // Get current local time
  time_t now = time(nullptr);
  struct tm t;
  memset(&t, 0, sizeof(t));
  #if defined(ESP8266) || defined(ESP32)
    // localtime_r is available on both cores
    localtime_r(&now, &t);
  #else
    // Native build fallback
    localtime_r(&now, &t);
  #endif

  // If time hasn't been set yet, show dashes "--:--" with blinking colon
  char buf[6]; // HH:MM + NUL
  if (t.tm_year < (2016 - 1900)) {
    buf[0] = '-'; buf[1] = '-';
    buf[2] = ':'; buf[3] = '-'; buf[4] = '-'; buf[5] = '\0';
  } else {
    int hh = t.tm_hour;
    int mm = t.tm_min;
    buf[0] = (char)('0' + (hh / 10));
    buf[1] = (char)('0' + (hh % 10));
    buf[2] = ':';
    buf[3] = (char)('0' + (mm / 10));
    buf[4] = (char)('0' + (mm % 10));
    buf[5] = '\0';
  }

  // Apply blinking colon
  if (!colonOn) {
    buf[2] = ' ';
  }

  // Clear and draw the string centered using Font4x6
  display->clear();
  drawString(buf);
}

void Clock::drawString(const char* s) {
  if (!s || !*s) return;

  const uint8_t maxWidth = display->width();
  const uint8_t maxHeight = display->height();

  // Compute total pixel width of the string in 4x6 with 1px spacing
  uint8_t len = 0;
  for (const char* p = s; *p; ++p) ++len;
  if (len == 0) return;
  const uint8_t glyphW = Font4x6::WIDTH;
  const uint8_t spacing = Font4x6::SPACING;
  uint8_t textW = (uint8_t)(len * glyphW + (len - 1) * spacing);

  uint8_t startX = 0;
  if (textW < maxWidth) {
    startX = (uint8_t)((maxWidth - textW) / 2);
  }

  uint8_t yOffset = 0;
  if (maxHeight > Font4x6::HEIGHT) {
    yOffset = (uint8_t)((maxHeight - Font4x6::HEIGHT) / 2);
  }

  uint8_t x = startX;
  for (const char* p = s; *p; ++p) {
    if (x + glyphW > maxWidth) break;
    Font4x6::Glyph g = Font4x6::glyphFor(*p);
    for (uint8_t cx = 0; cx < glyphW; ++cx) {
      uint8_t col = g.cols[cx];
      for (uint8_t ry = 0; ry < Font4x6::HEIGHT; ++ry) {
        bool on = ((col >> ry) & 0x01) != 0;
        if (on) {
          uint8_t y = (uint8_t)(yOffset + ry);
          if (y < maxHeight) {
            display->setPixel(x + cx, y, true);
          }
        }
      }
    }
    x = (uint8_t)(x + glyphW + spacing);
  }
}
