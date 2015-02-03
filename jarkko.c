#include "nibble.h"

#if 1
static void nibble_sort_word(unsigned long *w) {
  unsigned int b[16] = {0};
  unsigned long t = *w;
  for (int i = 0; i < 16; i++, t >>= 4) {
    b[t & 0xFUL]++;
  }
  for (int h = 60, i = 15; i >= 0; i--) {
    for (; b[i]; b[i]--, h -= 4) {
      t |= (unsigned long)i << h;
    }
  }
  *w = t;
}
#else
static void nibble_sort_word(unsigned long *w) {
  for (int h = 1; h < 16; h <<= 1) {
    unsigned long u = 0;
    unsigned long v = 0;
    int uc = 60;
    int vc = 60;
    for (int i = 60; i >= 0; i -= 4) {
      unsigned long n = ((*w & (0xFUL << i)) >> i);
      if ((n & (unsigned long)h)) {
        u |= n << uc;
        uc -= 4;
      } else {
        v |= n << vc;
        vc -= 4;
      }
    }
    *w = u | (v >> (60 - uc));
  }
}
#endif

void nibble_sort_jarkko(unsigned long *buf) {
  unsigned long *b = buf;
  for (int i = 0; i < 1024; i++) {
    nibble_sort_word(&b[i]);
  }
}
