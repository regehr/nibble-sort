#include "nibble.h"

static unsigned long nibble_sort_word(unsigned long word) {
  char nibble_counts[16] = {0};
  unsigned long result = 0;
  unsigned char nibble_index = 15;

  for (int i = 0; i < sizeof(word) * 2; ++i) {
    nibble_counts[word & 0x0F]++;
    word >>= 4;
  }

  for (unsigned long int i = 15; i > 0; --i) {
    while (nibble_counts[i]--) {
      result |= (i << nibble_index * 4);
      nibble_index--;
    }
  }

  return result;
}

void nibble_sort_mike(unsigned long *buf) {
  unsigned long *b = buf;
  for (int i = 0; i < 1024; i++) {
    buf[i] = nibble_sort_word(b[i]);
  }
}
