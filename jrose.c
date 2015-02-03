#include <assert.h>
// #include <stdlib.h>

#include "nibble.h"

#if MAIN
#include <stdio.h>
#include <string.h>
// For time
#include <CoreFoundation/CoreFoundation.h>
#endif

/// Reads nibble \p which of \p word and multiplies it by \p scale.
///
/// If \p which is known at compile-time and \p scale is a power of two,
/// this becomes a single shift and mask (at least in Clang).
static inline unsigned long read_nibble_scale(unsigned long word, uint8_t which,
                                              unsigned long scale) {
  return ((word >> (which * 4)) & 0xf) * scale;
}

/// Shift in the correct number of nibbles for the value \p which,
/// given the result so far in \p word and the count buckets in \p counts.
/// Intended for \p which to be known at compile time.
///
/// For detail on the "invert" part, see the implementation, below.
/// Otherwise, just know that you have to call this function with
/// both an even and an odd value for \p which to get the result
/// you need, rather than the result you deserve.
static inline unsigned long
shift_in_and_invert(unsigned long word, unsigned long counts, uint8_t which);

void nibble_sort_jrose(unsigned long buf[static 1024]) {
  for (uint16_t _ = 0; _ < 1024; _++) {
    unsigned long word = buf[_];

    // If all sixteen nibbles of the word are the same, don't do any work.
    // This costs us a branch, but (a) it should be well-predicted, and
    // (b) the implementation below relies on there being at most 15 of
    // any particular value.
    if (((word >> 60) | (word << 4)) == word)
      continue;

    // Bucket sort, using the sixteen nibbles of 'counts' as the buckets.
    // As always, bucket/nibble 'i' is at offset '4*i'.
    unsigned long counts = 0;
    counts += 1ul << read_nibble_scale(word, 0x0, 4);
    counts += 1ul << read_nibble_scale(word, 0x1, 4);
    counts += 1ul << read_nibble_scale(word, 0x2, 4);
    counts += 1ul << read_nibble_scale(word, 0x3, 4);
    counts += 1ul << read_nibble_scale(word, 0x4, 4);
    counts += 1ul << read_nibble_scale(word, 0x5, 4);
    counts += 1ul << read_nibble_scale(word, 0x6, 4);
    counts += 1ul << read_nibble_scale(word, 0x7, 4);
    counts += 1ul << read_nibble_scale(word, 0x8, 4);
    counts += 1ul << read_nibble_scale(word, 0x9, 4);
    counts += 1ul << read_nibble_scale(word, 0xA, 4);
    counts += 1ul << read_nibble_scale(word, 0xB, 4);
    counts += 1ul << read_nibble_scale(word, 0xC, 4);
    counts += 1ul << read_nibble_scale(word, 0xD, 4);
    counts += 1ul << read_nibble_scale(word, 0xE, 4);
    counts += 1ul << read_nibble_scale(word, 0xf, 4);

    // Compute the answer by shifting in successive values from the right.
    // We start with 0xFFF... and shift in the right number of Es, Ds, etc.
    // until we get through 1, then do one last shift for the zeros.
    // (We don't actually read the count for Fs ever, but there's not really
    // any way to avoid computing it above.)
    //
    // What's that "invert"? See the implementation, below.
    word = ~0ul;

    word = shift_in_and_invert(word, counts, 0xE);
    word = shift_in_and_invert(word, counts, 0xD);
    word = shift_in_and_invert(word, counts, 0xC);
    word = shift_in_and_invert(word, counts, 0xB);
    word = shift_in_and_invert(word, counts, 0xA);
    word = shift_in_and_invert(word, counts, 0x9);
    word = shift_in_and_invert(word, counts, 0x8);
    word = shift_in_and_invert(word, counts, 0x7);
    word = shift_in_and_invert(word, counts, 0x6);
    word = shift_in_and_invert(word, counts, 0x5);
    word = shift_in_and_invert(word, counts, 0x4);
    word = shift_in_and_invert(word, counts, 0x3);
    word = shift_in_and_invert(word, counts, 0x2);
    word = shift_in_and_invert(word, counts, 0x1);

    word <<= read_nibble_scale(counts, 0x0, 4);

    buf[_] = word;
  }
}

// Declared above, defined down here for a better narrative.
static inline unsigned long
shift_in_and_invert(unsigned long word, unsigned long counts, uint8_t which) {
  // How many nibbles do we want?
  // Actually, how many bits do we want? (hence the "times four")
  uint8_t countTimesFour = read_nibble_scale(counts, which, 4);

  // Make room in the result for the new bits.
  word <<= countTimesFour;

  // Compute a repeated value of the form 0x1111..., 0x2222..., etc...
  unsigned long repeatedNibble = (which * (~0ul / 0xf));
  // ...but then invert it if the value is even.
  // This means we only need to deal with 7-8 constant values
  // (-1 is easy) instead of 15 (skipping 0).
  // Clang will optimize this even if I write it with 'if' and '%',
  // but I got nervous about leaving in anything that looks like a branch.
  repeatedNibble ^= (which & 1) - 1;

  // Compute a mask of the value to preserve the existing
  // part of the result...
  unsigned long mask = ~0ul << countTimesFour;
  // ...and apply it to our constant.
  repeatedNibble |= mask;

  // XOR the value into our accumulated result.
  // If we're working with an odd value, we expect 'word' to be
  // negated from the real result (e.g. 0x0001 instead of 0xFFFE),
  // and so we invert and then shift in the new value (0xFFED).
  // If we're working with an even value, we expect 'word' to be
  // in its correct form (0xFFED), and afterwards we'll have an
  // inverted value (~0xFEDC, or 0x0123).
  // This is why we need to follow every even value with an odd one.
  return word ^ repeatedNibble;
}

#if MAIN

/// Reference implementation
int read_nibble(unsigned long w, int i) {
  assert(i >= 0 && i < 16);
  unsigned long res = w >> (i * 4);
  return res & 0xf;
}

/// Reference implementation
void write_nibble(unsigned long *w, int i, int v) {
  assert(i >= 0 && i < 16);
  unsigned long mask = 0xf;
  mask <<= (i * 4);
  *w &= ~mask;
  unsigned long prom = v;
  prom <<= (i * 4);
  *w |= prom;
}

/// Reference implementation
unsigned long nibble_sort_word(unsigned long arg) {
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

/// Reference implementation
void nibble_sort(unsigned long *buf) {
  for (int i = 0; i < 1024; i++)
    buf[i] = nibble_sort_word(buf[i]);
}

// Usage: nibble 0xbadbeef
int main(int argc, const char *argv[]) {
  assert(argc == 2);

  unsigned long buf[1024] = {};
  arc4random_buf(buf, sizeof(buf));
  buf[0] = strtol(argv[1], NULL, 16);

  unsigned long buf1[1024];
  memcpy(buf1, buf, sizeof(buf));
  unsigned long buf2[1024];
  memcpy(buf2, buf, sizeof(buf));

  printf("%016lx\n", buf[0]);

  CFAbsoluteTime start = CFAbsoluteTimeGetCurrent();
  nibble_sort(buf1);
  CFAbsoluteTime mid = CFAbsoluteTimeGetCurrent();
  nibble_sort_jrose(buf2);
  CFAbsoluteTime end = CFAbsoluteTimeGetCurrent();

  printf("%016lx at %fus\n", buf2[0], (end - mid) * USEC_PER_SEC);

  for (int i = 0; i < 1024; ++i) {
    if (buf1[i] != buf2[i]) {
      printf("FAIL %016lx -> %016lx (should be %016lx)\n", buf[i], buf2[i],
             buf1[i]);
      break;
    }
  }

  printf("%016lx at %fus (reference)\n", buf1[0], (mid - start) * USEC_PER_SEC);
}

#endif
