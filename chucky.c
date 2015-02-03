// Chucky Ellison
// freefour.com

#include "nibble.h"

#include <stdint.h>

#define NIBBLE_WIDTH 4
#define NUM_NIBBLES 16 // number of different nibbles there are (0--15)
#define BUF_LEN 1024

// this is a lookup table from nibbles x and counts y to value of y xs in a row.
// e.g., lookups[5][3] is 0x555
// there's NUM_NIBBLES + 1 different counts (0--16)
static uint64_t lookups[NUM_NIBBLES][NUM_NIBBLES + 1];

// initializes the table described above
// this could be done statically, or even entered manually into the source
static __attribute__((constructor)) void init_lookups() {
  for (int nibble = 0; nibble < NUM_NIBBLES; nibble++) {
    uint64_t val = 0;
    for (int count = 0; count < NUM_NIBBLES + 1; count++) {
      lookups[nibble][count] = val;
      val = (val << NIBBLE_WIDTH) + nibble;
    }
  }
}

// given a 64 bit word, this creates its nibble histogram
static void build_histogram(uint64_t arg, int histogram[static NUM_NIBBLES]) {
  for (int i = 0; i < NUM_NIBBLES; i++) {
    int nibble = arg & 0xf;
    histogram[nibble]++;
    arg >>= NIBBLE_WIDTH;
  }
}

// given a nibble-histogram of a 64 bit word, this sorts its nibbles
// with (unsigned) smaller values towards the small end of the word.
// The nibble sort of 0xbadbeef is 0xfeedbba000000000.
static uint64_t sort_using_histogram(const int histogram[static NUM_NIBBLES]) {
  uint64_t res = 0;

  for (int nibble = NUM_NIBBLES - 1; nibble >= 0; nibble--) {
    int count = histogram[nibble];
    res = (res << (NIBBLE_WIDTH * count)) + lookups[nibble][count];
  }

  return res;
}

// nibble sorts the 64 bit words of a buffer of length BUF_LEN
void nibble_sort_chucky(unsigned long *buf) {
  // really only needs to be done once, or could be done statically
  // init_lookups();

  int histograms[BUF_LEN][NUM_NIBBLES] = {{0}};

  // first we compute nibble histograms for each of the 64 bit words
  // we do these all at once because it's faster than doing it before every sort
  for (int i = 0; i < BUF_LEN; i++) {
    build_histogram(buf[i], histograms[i]);
  }

  // now we use those histograms to sort the words
  for (int i = 0; i < BUF_LEN; i++) {
    buf[i] = sort_using_histogram(histograms[i]);
  }
}
