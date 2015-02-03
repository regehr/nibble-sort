#include "nibble.h"

static void inline nibble_sort_david(uint64_t *word) {
  unsigned char count[16] = {
      0,
  };
  uint64_t w = *word;
  int i, j;

  for (i = 0; i < 16; i++) {
    unsigned char nibble = w & 0xf;
    count[nibble] += 1;
    w >>= 4;
  }

  w = 0;
  for (i = 15; i >= 0; i--) {
    for (j = count[i]; j > 0; j--) {
      w <<= 4;
      w = w | i;
    }
  }
  *word = w;
}

void nibble_sort_mentre(unsigned long *buf) {
  unsigned long *b = buf;
  for (int i = 0; i < 1024; i++) {
    nibble_sort_david(&b[i]);
  }
}
