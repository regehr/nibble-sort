#include "nibble.h"

static int read_nibble(uint64_t w, int i) { return (w >> (i * 4)) & 0xf; }

static uint64_t nibble_sort_word(uint64_t arg) {
  uint8_t counts[16] = {0};

  for (int i = 0; i < 16; ++i) {
    counts[read_nibble(arg, i)]++;
  }

  uint64_t ret = 0;
  int pos = 0;

  for (int c = 0; c < 16; c++) {
    for (int i = 0; i < counts[c]; i++) {
      ret |= ((uint64_t)c) << pos;
      pos += 4;
    }
  }

  return ret;
}

void nibble_sort_rogers(uint64_t *buf) {
  for (int i = 0; i < 1024; i++)
    buf[i] = nibble_sort_word(buf[i]);
}
