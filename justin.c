#include "nibble.h"

#include <assert.h>

static int read_nibble(unsigned long w, int i) {
  assert(i >= 0 && i < 16);
  unsigned long res = w >> (i * 4);
  return res & 0xf;
}

static void write_nibble(unsigned long *w, int i, int v) {
  assert(i >= 0 && i < 16);
  unsigned long mask = 0xf;
  mask <<= (i * 4);
  *w &= ~mask;
  unsigned long prom = v;
  prom <<= (i * 4);
  *w |= prom;
}

static unsigned long nibble_sort(unsigned long arg) {
  int i, j;
  unsigned long counter[16] = {0};

  for (i = 0; i < 16; i++) {
    counter[read_nibble(arg, i)]++;
  }

  // Loop through all the buckets
  for (i = 0, j = 0; i < 16; i++) {
    while (counter[i]--) {
      write_nibble(&arg, j++, i);
    }
  }

  return arg;
}

void nibble_sort_justin(unsigned long *buf) {
  unsigned long *b = buf;
  for (int i = 0; i < 1024; i++) {
    buf[i] = nibble_sort(b[i]);
  }
}
