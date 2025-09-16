#pragma once

#define PIN_DIN D7
#define PIN_CLK D5
#define PIN_CS  D8
#define NUM_DEVICES 4

#define LED_MATRIX_ROWS 8
#define LED_MATRIX_ROWS_STR "8"
#define LED_MATRIX_COLS 32
#define LED_MATRIX_COLS_STR "32"
#define LED_MATRIX_BRIGHTNESS_MIN 0
#define LED_MATRIX_BRIGHTNESS_MAX 15

// Wi-Fi credentials are provided via PlatformIO build flags
// e.g., -D WIFI_SSID=... and -D WIFI_PASSWORD=...
#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif
