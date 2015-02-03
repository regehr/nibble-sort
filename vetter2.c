#include "nibble.h"

#define TEST_SIZE 1024

void nibble_sort_vetter2(unsigned long long *buf) {
  for (int num = 0; num < TEST_SIZE; num++) {
    unsigned long long value = buf[num];
    char count[16] = {0};
    for (int i = 0; i < 16; i++, value >>= 4)
      count[value & 0xF]++;

    unsigned long long result = 0;
    int pos = 0;
    for (int i = 0; i < 16; i++) {
      int shift = count[i] * 4;
      unsigned long long data = (0x1111111111111111ULL * i) << pos;
      result ^= data ^ (data << shift);
      pos += shift;
    }

    if (result != 0)
      buf[num] = result;
  }
}
