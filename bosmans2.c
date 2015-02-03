#include "nibble.h"

static unsigned long
nibble_sort_word_mkbosmans_insertionsort(unsigned long arg) {
  unsigned long pos = 0;
  unsigned long res = 0;
  for (int i = 0; i < 16; i++) {
    unsigned long n = arg & 0xf;
    unsigned long n_pos = (pos >> (n * 4)) & 0xf;
    unsigned long mask = 0xffffffffffffffff << (n_pos * 4);
    res = (res & mask) << 4 | n << (n_pos * 4) | (res & ~mask);
    pos += 0x1111111111111111 << (n * 4);
    arg >>= 4;
  }
  return res;
}

void nibble_sort_bosmans2(unsigned long *buf) {
  for (int i = 0; i < 1024; i++)
    buf[i] = nibble_sort_word_mkbosmans_insertionsort(buf[i]);
}
