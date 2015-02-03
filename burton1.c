#include "nibble.h"

#include <string.h>

static unsigned long nibble_sort_word(unsigned long arg, int counts[16]) {
  for (int i = 0; i < 16; i++) {
    counts[arg & 0xf]++;
    arg >>= 4;
  }
  for (int i = 15; i >= 0; i--) {
    while (counts[i] != 0) {
      arg <<= 4;
      arg |= i;
      counts[i]--;
    }
  }
  return arg;
}

#define TEST_SIZE 1024

void nibble_sort_burton1(unsigned long *buf) {
  int counts[16];
  memset(counts, 0, 16 * sizeof(int));
  for (int i = 0; i < TEST_SIZE; i++) {
    buf[i] = nibble_sort_word(buf[i], counts);
  }
}
