#include "nibble.h"

// Odd-even sorting network (http://en.wikipedia.org/wiki/Odd%E2%80%93even_sort)

static unsigned long swap(unsigned long x, unsigned long y,
                          unsigned long mask) {
  unsigned long d = y - x;
  unsigned long m = (mask & d) >> 4;
  unsigned long max = (x & m) | (y & ~m);
  unsigned long min = (x & ~m) | (y & m);
  unsigned long r = (max << 4) | min;
  return r;
}

static unsigned long sort_even(unsigned long w) {
#if 0
    unsigned long x =  w       & 0x0f0f0f0f0f0f0f0f;
    unsigned long y = (w >> 4) & 0x0f0f0f0f0f0f0f0f;
    return swap(x, y, 0xf0f0f0f0f0f0f0f0);
#else
  // faster but somewhat obfuscated
  unsigned long x = w & 0x0f0f0f0f0f0f0f0f;
  unsigned long y = (w >> 4) & 0x0f0f0f0f0f0f0f0f;
  unsigned long d = ((x - y) >> 4) & (x ^ y);
  unsigned long r = ((x ^ d) << 4) | (y ^ d);
  return r;
#endif
}

static unsigned long sort_odd(unsigned long w) {
#if 0
    unsigned long x =  w       & 0xf0f0f0f0f0f0f0ff;
    unsigned long y = (w >> 4) & 0xf0f0f0f0f0f0f0f0;
    return swap(x, y, 0x0f0f0f0f0f0f0f0f);
#elif 0
  unsigned long b = w & 0xf00000000000000f;
  unsigned long x = w & 0xf0f0f0f0f0f0f0f0;
  unsigned long y = (w >> 4) & 0xf0f0f0f0f0f0f0f0;
  unsigned long d = ((x - y) >> 4) & (x ^ y);
  unsigned long r = ((x ^ d) << 4) | (y ^ d);
  return r | b;
#elif 1
  // found with the help of the STOKE superoptimizer
  unsigned long x = w & 0xf0f0f0f0f0f0f0f0;
  unsigned long wx = w - x;
  x <<= 4;
  unsigned long wxw = (wx ^ x) & ((wx - x) >> 4);
  return wxw ^ w ^ (wxw >> 4);
#endif
}

static unsigned long nibble_sort_word(unsigned long x) {
#if 0
    for (int i = 0; i < 8; ++i) {
	x = sort_even(x);
	x = sort_odd(x);
    }
#else
  // unroll manually
  x = sort_even(x);
  x = sort_odd(x);
  x = sort_even(x);
  x = sort_odd(x);
  x = sort_even(x);
  x = sort_odd(x);
  x = sort_even(x);
  x = sort_odd(x);
  x = sort_even(x);
  x = sort_odd(x);
  x = sort_even(x);
  x = sort_odd(x);
  x = sort_even(x);
  x = sort_odd(x);
  x = sort_even(x);
  x = sort_odd(x);
#endif
  return x;
}

void nibble_sort_falk(unsigned long *buf) {
  for (int i = 0; i < 1024; i++)
    buf[i] = nibble_sort_word(buf[i]);
}
