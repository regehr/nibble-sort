#include "nibble.h"

#define TEST_SIZE 1024

void nibble_sort_vetter1(unsigned long long *buf) {
  for (int num = 0; num < TEST_SIZE; num++) {
    unsigned long long value = buf[num];
    unsigned long long count = 0;
    for (int i = 0; i < 16; i++, value >>= 4)
      count += 1ULL << (4 * (value & 0xF));

    unsigned long long result = 0;
    int pos = 0;
    for (int i = 0; i < 16; i++, count >>= 4) {
      int shift = (count & 0xF) * 4;
      unsigned long long data = (0x1111111111111111ULL * i) << pos;
      result ^= data ^ (data << shift);
      pos += shift;
    }

    if (pos == 64)
      buf[num] = result;
  }
}
