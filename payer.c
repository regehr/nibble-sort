#include "nibble.h"

static unsigned long nibble_sort_word_payer(unsigned long arg) {
  unsigned long counts = 0;

  // printf("value: %16lx\n", arg);
  counts += 1ul << ((arg & 0xF) << 2);
  counts += 1ul << ((arg & 0xF0) >> 2);
  counts += 1ul << ((arg & 0xF00) >> 6);
  counts += 1ul << ((arg & 0xF000) >> 10);
  counts += 1ul << ((arg & 0xF0000) >> 14);
  counts += 1ul << ((arg & 0xF00000) >> 18);
  counts += 1ul << ((arg & 0xF000000) >> 22);
  counts += 1ul << ((arg & 0xF0000000) >> 26);
  counts += 1ul << ((arg & 0xF00000000) >> 30);
  counts += 1ul << ((arg & 0xF000000000) >> 34);
  counts += 1ul << ((arg & 0xF0000000000) >> 38);
  counts += 1ul << ((arg & 0xF00000000000) >> 42);
  counts += 1ul << ((arg & 0xF000000000000) >> 46);
  counts += 1ul << ((arg & 0xF0000000000000) >> 50);
  counts += 1ul << ((arg & 0xF00000000000000) >> 54);
  counts += 1ul << ((arg & 0xF000000000000000) >> 58);

  /* 16x the same value (therefore either only 1 bit is set or 0
   * bits are set due to overflow for 16) */
  if (counts == 0 || (counts & (counts - 1)) == 0)
    return arg;

  /* now walk through the counts and produce the output
     by sifting through the numbers one after the other
     we are abusing a bunch of registers but that's fine
     as x64 has plenty */
  unsigned long cur_mask = 0xF000000000000000;
  unsigned long cur_nr = 0xF;
  unsigned long ret = 0;
  while (cur_mask != 0) {
    if ((counts & cur_mask) == 0) {
      cur_mask = cur_mask >> 4;
      cur_nr--;
      continue;
    }
    ret = (ret << 4) | cur_nr;
    counts -= 1ull << (cur_nr << 2);
  }
  // printf("sort:  %16lx\n", ret);
  return ret;
}

void nibble_sort_payer(unsigned long *buf) {
  unsigned long *b = buf;
  // double *b = buf;
  for (int i = 0; i < 1024; i++) {
    buf[i] = nibble_sort_word_payer(b[i]);
  }
}
