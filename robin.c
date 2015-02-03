#include "nibble.h"

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

/* My code */

static void write_nibble_or(unsigned long *w, int i, uint32_t v) {
  assert(i >= 0 && i < 16);
  unsigned long prom = v;
  prom <<= (i * 4);
  *w |= prom;
}

static void comparator_swap(uint32_t *i, uint32_t *j) {
  if (*j < *i) {
    uint32_t tmp = *i;
    *i = *j;
    *j = tmp;
  }
}

static uint32_t tableLow[256] __attribute__((aligned(64)));
static uint32_t tableHigh[256] __attribute__((aligned(64)));

// saves hand-coding the tables
__attribute__((constructor)) static void init_tablesLowHigh() {
  for (uint32_t i = 0; i < 16; ++i) {
    for (uint32_t j = 0; j < 16; ++j) {
      tableLow[i * 16 + j] = i < j ? i : j;
      tableHigh[i * 16 + j] = i < j ? j : i;
    }
  }
}

static unsigned long my_nibble_sort_word(unsigned long arg) {
  // First level of the sorting network is done by
  // lookup tables.
  uint32_t n0 = tableLow[arg & 0xff];
  uint32_t n1 = tableHigh[arg & 0xff];
  arg >>= 8;
  uint32_t n2 = tableLow[arg & 0xff];
  uint32_t n3 = tableHigh[arg & 0xff];
  arg >>= 8;
  uint32_t n4 = tableLow[arg & 0xff];
  uint32_t n5 = tableHigh[arg & 0xff];
  arg >>= 8;
  uint32_t n6 = tableLow[arg & 0xff];
  uint32_t n7 = tableHigh[arg & 0xff];
  arg >>= 8;
  uint32_t n8 = tableLow[arg & 0xff];
  uint32_t n9 = tableHigh[arg & 0xff];
  arg >>= 8;
  uint32_t n10 = tableLow[arg & 0xff];
  uint32_t n11 = tableHigh[arg & 0xff];
  arg >>= 8;
  uint32_t n12 = tableLow[arg & 0xff];
  uint32_t n13 = tableHigh[arg & 0xff];
  arg >>= 8;
  uint32_t n14 = tableLow[arg & 0xff];
  uint32_t n15 = tableHigh[arg & 0xff];

  // Sorting network from
  // http://www.cs.brandeis.edu/~hugues/graphics/green.gif
  comparator_swap(&n0, &n2);
  comparator_swap(&n4, &n6);
  comparator_swap(&n8, &n10);
  comparator_swap(&n12, &n14);
  comparator_swap(&n1, &n3);
  comparator_swap(&n5, &n7);
  comparator_swap(&n9, &n11);
  comparator_swap(&n13, &n15);
  comparator_swap(&n0, &n4);
  comparator_swap(&n1, &n5);
  comparator_swap(&n2, &n6);
  comparator_swap(&n3, &n7);
  comparator_swap(&n8, &n12);
  comparator_swap(&n9, &n13);
  comparator_swap(&n10, &n14);
  comparator_swap(&n11, &n15);
  comparator_swap(&n0, &n8);
  comparator_swap(&n1, &n9);
  comparator_swap(&n2, &n10);
  comparator_swap(&n3, &n11);
  comparator_swap(&n4, &n12);
  comparator_swap(&n5, &n13);
  comparator_swap(&n6, &n14);
  comparator_swap(&n7, &n15);
  comparator_swap(&n5, &n10);
  comparator_swap(&n6, &n9);
  comparator_swap(&n3, &n12);
  comparator_swap(&n13, &n14);
  comparator_swap(&n7, &n11);
  comparator_swap(&n1, &n2);
  comparator_swap(&n4, &n8);
  comparator_swap(&n1, &n4);
  comparator_swap(&n7, &n13);
  comparator_swap(&n2, &n8);
  comparator_swap(&n11, &n14);
  comparator_swap(&n2, &n4);
  comparator_swap(&n5, &n6);
  comparator_swap(&n9, &n10);
  comparator_swap(&n11, &n13);
  comparator_swap(&n3, &n8);
  comparator_swap(&n7, &n12);
  comparator_swap(&n6, &n8);
  comparator_swap(&n10, &n12);
  comparator_swap(&n3, &n5);
  comparator_swap(&n7, &n9);
  comparator_swap(&n3, &n4);
  comparator_swap(&n5, &n6);
  comparator_swap(&n7, &n8);
  comparator_swap(&n9, &n10);
  comparator_swap(&n11, &n12);
  comparator_swap(&n6, &n7);
  comparator_swap(&n8, &n9);

  uint64_t ret = 0;
  write_nibble_or(&ret, 0, n0);
  write_nibble_or(&ret, 1, n1);
  write_nibble_or(&ret, 2, n2);
  write_nibble_or(&ret, 3, n3);
  write_nibble_or(&ret, 4, n4);
  write_nibble_or(&ret, 5, n5);
  write_nibble_or(&ret, 6, n6);
  write_nibble_or(&ret, 7, n7);
  write_nibble_or(&ret, 8, n8);
  write_nibble_or(&ret, 9, n9);
  write_nibble_or(&ret, 10, n10);
  write_nibble_or(&ret, 11, n11);
  write_nibble_or(&ret, 12, n12);
  write_nibble_or(&ret, 13, n13);
  write_nibble_or(&ret, 14, n14);
  write_nibble_or(&ret, 15, n15);
  return ret;
}

#define TEST_SIZE 1024

void nibble_sort_robin(unsigned long *buf) {
  for (int i = 0; i < TEST_SIZE; i++) {
    buf[i] = my_nibble_sort_word(buf[i]);
  }
}
