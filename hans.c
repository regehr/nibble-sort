#include <stdint.h>

#include "nibble.h"

static uint64_t nibble_sort_word(uint64_t arg) {
  const uint64_t ones = 0x1111111111111111ULL;
  if (arg == (arg & 0xf) * ones) {
    return arg;
  }

  uint64_t count = 0;
  for (int i = 0; i < 16; i++) {
    count += 1ULL << (4 * (arg & 0xf));
    arg >>= 4;
  }

  uint64_t mask = ~0ULL;
  uint64_t result = 0;

  for (int i = 0; i < 16; i++) {
    int n = count & 0xf;
    count >>= 4;
    result = (result & ~mask) | ((ones & mask) * i);
    mask <<= 4 * n;
  }

  return result;
}

void nibble_sort_hans(uint64_t *buf) {
  for (int i = 0; i < 1024; i++) {
    buf[i] = nibble_sort_word(buf[i]);
  }
}
