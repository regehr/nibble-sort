#include "nibble.h"

/** -*- c++ -*-

    Program which sorts nibbles in a 64-bit word in decreasing order
    (where the least-significant nibble contains the lowest-value)

    An attempted solution of John Regehr's programming contest, reference:
      http://blog.regehr.org/archives/1213

    Had a nice time doing it, and learned a lot.  Thanks!

    --------------------------------------------------------
    Copyright 2015 Karim J. Naqvi

    Distributable under the GPLv3, or any later version.  See
    <http://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

// Useful debugging macros.
#define dump64(x) printf(#x " 0x%016lx\n", x);

#define dump(x) printf(#x ": %d\n", x);

static uint64_t mask_array[] = {
    0x0uL,                0x1111111111111111uL, 0x2222222222222222uL,
    0x3333333333333333uL, 0x4444444444444444uL, 0x5555555555555555uL,
    0x6666666666666666uL, 0x7777777777777777uL, 0x8888888888888888uL,
    0x9999999999999999uL, 0xaaaaaaaaaaaaaaaauL, 0xbbbbbbbbbbbbbbbbuL,
    0xccccccccccccccccuL, 0xdddddddddddddddduL, 0xeeeeeeeeeeeeeeeeuL,
    0xffffffffffffffffuL};

/** I don't know what to call this sort, but it's the fastest one I've
 * created so far.  And it has some tricky stuff.
 */
static void tricky_sort(uint64_t *buf) {
  uint64_t result = 0;
  int num_nibbles_filled = 0;

  for (uint64_t nibble = 0; nibble < 16; ++nibble) {
    // any matching nibble in the buffer will become binary 1111
    uint64_t match = ~(*buf ^ mask_array[nibble]);

    // AND the four bits of each nibble together to get a single 1 IFF
    //  the original nibble was all 1's
    match =
        (match & 0x5555555555555555uL) & ((match >> 1) & 0x5555555555555555uL);
    match =
        (match & 0x3333333333333333uL) & ((match >> 2) & 0x3333333333333333uL);

    while (match != 0) {
      match &= (match - 1); // zap lsb
      result |= nibble << (((uint64_t)num_nibbles_filled) * 4);
      ++num_nibbles_filled;
    }
  }
  *buf = result;
}

////////////////////////////////////////////////////////////////
// The following global declarations are part of the interface used by
// the test driver code in test_drive.c.  Accordingly these functions
// must also be prototyped in nibble.h, which is included in the test
// drive routine.
////////////////////////////////////////////////////////////////
void nibble_sort_karim(uint64_t *buf) {
  for (int i = 0; i < 1024; ++i)
    tricky_sort(&buf[i]);
}

// other trial nibble sorting routines removed ...

////////////////////////////////////////////////////////////////
// The rest of this file is just self-test routines.
#ifdef TEST
typedef struct test_vector {
  uint64_t in_out;
  uint64_t expected;
} Test_vector_t;

typedef void (*sort_func_t)(uint64_t *);

void test_sort(sort_func_t sort_func) {
  Test_vector_t vec[] = {
      {0x6ea300c39b617c25, 0xeccba97665332100},
      {0x1234567890abcde, 0xedcba98765432100},
      {0x4444444444444444, 0x4444444444444444},
      {0x7affcc3e5aa3bc29, 0xffecccbaaa975332},
  };

  for (int i = 0; i < sizeof(vec) / sizeof(vec[0]); ++i) {

    // store this, because it'll be trashed in the call
    uint64_t input = vec[i].in_out;

    sort_func(&vec[i].in_out);

    if (vec[i].in_out != vec[i].expected) {
      printf("error: rec#: %d\n"
             "input:     0x%016lx\n"
             "expected:  0x%016lx\n"
             "got:       0x%016lx\n",
             i, input, vec[i].expected, vec[i].in_out);
      assert(0);
    }
  }
}

int main(void) {
  test_sort(tricky_sort);
  return 0;
}
#endif // TEST
