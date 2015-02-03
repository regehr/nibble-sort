#include <assert.h>

#include "nibble.h"

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

static unsigned long nibble_sort_word_ref(unsigned long arg) {
  for (int i = 0; i < 16; ++i) {
    int min = i;
    for (int j = i + 1; j < 16; ++j) {
      if (read_nibble(arg, j) < read_nibble(arg, min))
        min = j;
    }
    if (min != i) {
      int tmp = read_nibble(arg, i);
      write_nibble(&arg, i, read_nibble(arg, min));
      write_nibble(&arg, min, tmp);
    }
  }
  return arg;
}

void nibble_sort_ref(unsigned long *buf) {
  unsigned long *b = buf;
  for (int i = 0; i < 1024; i++) {
    buf[i] = nibble_sort_word_ref(b[i]);
  }
}
