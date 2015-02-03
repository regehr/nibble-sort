#include "nibble.h"

static unsigned long nibble_sort_word(unsigned long arg) {
  static const unsigned long tab[17] = {
      0x0000000000000000UL, 0x0000000000000001UL, 0x0000000000000011UL,
      0x0000000000000111UL, 0x0000000000001111UL, 0x0000000000011111UL,
      0x0000000000111111UL, 0x0000000001111111UL, 0x0000000011111111UL,
      0x0000000111111111UL, 0x0000001111111111UL, 0x0000011111111111UL,
      0x0000111111111111UL, 0x0001111111111111UL, 0x0011111111111111UL,
      0x0111111111111111UL, 0x1111111111111111UL,
  };
  char counts[16] = {};
  for (int i = 0; i < 16; ++i) {
    counts[arg & 15]++;
    arg >>= 4;
  }
  unsigned long result = 0;
  for (int i = 15; i >= 0; --i) {
    int c = counts[i];
    result <<= 4 * c;
    result |= i * tab[c];
  }
  return result;
}

void nibble_sort_pdewacht1(unsigned long *buf) {
  for (int i = 0; i < 1024; i++)
    buf[i] = nibble_sort_word(buf[i]);
}
