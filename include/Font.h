#pragma once

#include <stdint.h>

namespace Font3x5 {

// Basic 3x5 pixel font
// - Coordinates: bit 0 = top row, bit 4 = bottom row
// - Each glyph is 3 columns wide; add 1 column spacing between glyphs
static constexpr uint8_t WIDTH = 3;
static constexpr uint8_t HEIGHT = 5;
static constexpr uint8_t SPACING = 1;

struct Glyph {
  uint8_t cols[WIDTH];
};

// Return glyph columns for a character.
// Unknown characters map to '?'.
inline Glyph glyphFor(char ch) {
  char c = ch;
  switch (c) {
    // Space and simple punctuation
    case ' ': return {{0x00, 0x00, 0x00}};                               // "   "
    case '.': return {{0x00, 0x10, 0x00}};                               //   ·
    case ',': return {{0x10, 0x08, 0x00}};                               //  ,
    case ':': return {{0x00, 0x0A, 0x00}};                               //  :
    case '-': return {{0x04, 0x04, 0x04}};                               //  -
    case '+': return {{0x04, 0x0E, 0x04}};                               //  +
    case '=': return {{0x0A, 0x0A, 0x0A}};                               //  =
    case '!': return {{0x00, 0x17, 0x00}};                               //  !
    case '?': return {{0x01, 0x15, 0x03}};                               //  ?
    case '/': return {{0x18, 0x04, 0x03}};                               //  /

    // Digits 0-9
    case '0': return {{0x1F, 0x11, 0x1F}};
    case '1': return {{0x12, 0x1F, 0x10}};
    case '2': return {{0x1D, 0x15, 0x17}};
    case '3': return {{0x15, 0x15, 0x1F}};
    case '4': return {{0x07, 0x04, 0x1F}};
    case '5': return {{0x17, 0x15, 0x1D}};
    case '6': return {{0x1F, 0x15, 0x1D}};
    case '7': return {{0x01, 0x1D, 0x03}};
    case '8': return {{0x1F, 0x15, 0x1F}};
    case '9': return {{0x17, 0x15, 0x1F}};

    // Letters A-Z with lowercase fallthrough
    case 'A': case 'a': return {{0x1E, 0x15, 0x1E}};
    case 'B': case 'b': return {{0x1F, 0x15, 0x0A}};
    case 'C': case 'c': return {{0x0E, 0x11, 0x11}};
    case 'D': case 'd': return {{0x1F, 0x11, 0x0E}};
    case 'E': case 'e': return {{0x1F, 0x15, 0x11}};
    case 'F': case 'f': return {{0x1F, 0x05, 0x01}};
    case 'G': case 'g': return {{0x0E, 0x11, 0x1D}};
    case 'H': case 'h': return {{0x1F, 0x04, 0x1F}};
    case 'I': case 'i': return {{0x11, 0x1F, 0x11}};
    case 'J': case 'j': return {{0x01, 0x11, 0x0F}};
    case 'K': case 'k': return {{0x1F, 0x0A, 0x11}};
    case 'L': case 'l': return {{0x1F, 0x10, 0x10}};
    case 'M': case 'm': return {{0x1F, 0x06, 0x1F}};
    case 'N': case 'n': return {{0x1F, 0x06, 0x1F}}; // 3-wide N approximated
    case 'O': case 'o': return {{0x1F, 0x11, 0x1F}};
    case 'P': case 'p': return {{0x1F, 0x05, 0x02}};
    case 'Q': case 'q': return {{0x0F, 0x09, 0x1F}};
    case 'R': case 'r': return {{0x1F, 0x0D, 0x12}};
    case 'S': case 's': return {{0x06, 0x15, 0x0D}};
    case 'T': case 't': return {{0x01, 0x1F, 0x01}};
    case 'U': case 'u': return {{0x1F, 0x10, 0x1F}};
    case 'V': case 'v': return {{0x0F, 0x10, 0x0F}};
    case 'W': case 'w': return {{0x1F, 0x0C, 0x1F}};
    case 'X': case 'x': return {{0x1B, 0x04, 0x1B}};
    case 'Y': case 'y': return {{0x03, 0x1C, 0x03}};
    case 'Z': case 'z': return {{0x19, 0x15, 0x13}};

    default:  return {{0x01, 0x15, 0x03}}; // '?'
  }
}

} // namespace Font3x5

// 4x6 font derived from 3x5 by simple scaling
namespace Font4x6 {

static constexpr uint8_t WIDTH = 4;
static constexpr uint8_t HEIGHT = 6;
static constexpr uint8_t SPACING = 1;

struct Glyph {
  uint8_t cols[WIDTH];
};

inline uint8_t scaleCol5to6(uint8_t col5) {
  uint8_t col6 = 0;
  for (uint8_t r = 0; r < 6; ++r) {
    uint8_t src = (uint8_t)((r * 5) / 6); // 0,0,1,2,3,4
    if ((col5 >> src) & 0x01) col6 |= (1u << r);
  }
  return col6;
}

inline Glyph glyphFor(char ch) {
  auto g3 = Font3x5::glyphFor(ch);
  Glyph g6;
  uint8_t c0 = scaleCol5to6(g3.cols[0]);
  uint8_t c1 = scaleCol5to6(g3.cols[1]);
  uint8_t c2 = scaleCol5to6(g3.cols[2]);
  g6.cols[0] = c0;
  g6.cols[1] = c1;
  g6.cols[2] = c1; // widen by duplicating middle column
  g6.cols[3] = c2;
  return g6;
}

} // namespace Font4x6
