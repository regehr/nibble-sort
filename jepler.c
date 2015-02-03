#include "nibble.h"

/*
Copyright © 2015 Jeff Epler

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgement in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

// optimized counting nibble sort.

#include <stdint.h>

// tunable: number of nibbles to count at a time - 3 is best on my system
// (i5-3320M) Each level takes 4x the memory of the one before, 3 is 32kB,
// which fits in L1 cache.  Plausible range is 2..5
#define NN (3)

// Number of bits at a time
#define N (4 * NN)

// Number of loops per word
#define M ((64 + N - 1) / N)

// If the number of nibble per iteration is not a power of two, the last
// iteration sees extra upper zero nibbles, which have to be accounted for.
// Starting the count with FUDGE extra zero nibbles does this, relying on
// unsigned integer modulo arithmetic
#define FUDGE (16 - (M * NN) % 16)

// This table holds the precomputed nibble counts for N-bit words
static uint64_t nibble_count[1 << N] __attribute__((aligned(64)));

// This table holds values that help insert repetitions of a pair of nibbles;
// the first index is the nibble pair (0 = nibbles 10, 1 = nibbles 32, etc).
// the second index is the packed count of each nibble.
static uint64_t nibble_insert[8][16 * 16] __attribute__((aligned(64)));

static uint64_t slow_nibble_inserter(uint64_t v, uint64_t n, int r) {
  for (int i = 0; i < r; i++)
    v = (v << 4) | n;
  return v;
}

// Fill out the nibble_count and nibble_insert tables.  The gcc extension
// __attribute__ allows the tables to be filled before the start of main
// similarly to a C++ object with a nontrivial constructor
__attribute__((constructor)) static void init_tables() {
  for (int i0 = 0; i0 < (1 << N); i0++) {
    int i = i0;
    uint64_t k = 0;
    for (int j = 0; j < NN; j++) {
      int l = i & 0xf;
      i >>= 4;
      int sh = (15 - l) * 4;
      k = k + (UINT64_C(1) << sh);
    }
    nibble_count[i0] = k;
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 16; j++) {
      uint64_t v = slow_nibble_inserter(0, 2 * i + 1, j);
      for (int k = 0; k < 16; k++) {
        nibble_insert[i][k * 16 + j] = slow_nibble_inserter(v, 2 * i, k);
      }
    }
  }
}

// Step 1 of counting sort: count the number of each nibble.  Using the
// nibble_count table, count NN nibbles at each iteration, keeping the count of
// each nibble value in a nibble of the 'counts' word.  As a special case,
// all-nibbles-same numbers are returned unchanged.  These numbers have 16
// repetitions of the same nibble, and would overflow the counting nibble.
static uint64_t count_nibbles(uint64_t word) {
  if ((word << 4 | word >> 60) == word)
    return word;
  uint64_t counts = ((uint64_t)(FUDGE)) << 60;
  for (int i = 0; i < M; i++) {
    counts += nibble_count[(word >> i * N) & ((1 << N) - 1)];
  }
  return counts;
}

// Step 2 of counting sort: fill the output according to the already counted
// reptition.  Using the nibble_insert table, any number of repetitions of two
// nibbles are inserted by a single operation.  As a special case,
// all-nibbles-same numbers are returned unchanged, completing the required
// special-case handling.
static uint64_t fill_nibbles(uint64_t counts) {
  if ((counts << 4 | counts >> 60) == counts)
    return counts;
  uint64_t w = 0;
  for (int i = 7; i >= 0; i--) {
    int l = (counts >> (8 * (7 - i))) & 0xff;
    int s = (l & 0xf) + (l >> 4);
    w = (w << (4 * s)) | nibble_insert[i][l];
  }
  return w;
}

#define R1(x) (x)
#define R2(x) ((x), (x))
#define R4(x) R2(R2(x))
#define R8(x) R4(R2(x))
#define R16(x) R4(R4(x))
#define R32(x) R8(R4(x))

// Do the nibble sort using two passes through the buffer.  If L1 cache was big
// enough to hold both tables at once, it might be better to #define ONE_STAGE
// but on my system, this is not the case.
//
// The best unroll (repetition) counts were determined by hand.  Presumably,
// gcc is internally using the unrolled version for better scheduling.
void nibble_sort_jepler(uint64_t *buf) {
#undef ONE_STAGE
#ifdef ONE_STAGE
  for (int i = 0; i < 1024; i++)
    buf[i] = fill_nibbles(count_nibbles(buf[i]));
#else
  for (int i = 0; i < 1024;) {
    R16((buf[i] = count_nibbles(buf[i]), i++));
  }
  for (int i = 0; i < 1024;) {
    R16((buf[i] = fill_nibbles(buf[i]), i++));
  }
#endif
}
