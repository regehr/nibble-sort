#include "nibble.h"

#define BUF_SIZE 1024

void nibble_sort_vlad(unsigned long *pv) {
  unsigned char c[16] = {};
  unsigned long tmp[2];
  for (register unsigned long *e = pv + BUF_SIZE; pv != e; ++pv) {
    register unsigned long v = *pv;

    tmp[0] = v & 0xF0F0F0F0F0F0F0F0;
    tmp[0] >>= 4;
    tmp[1] = v & 0x0F0F0F0F0F0F0F0F;
    unsigned char *p = (unsigned char *)tmp;
    register int n;
    for (n = 15; n >= 0; --n) {
      ++c[*p++];
    }
    for (n = 15; n >= 0; --n) {
      while (c[n]) {
        v = (v << 4) | n;
        --c[n];
      }
    }
    *pv = v;
  }
}
