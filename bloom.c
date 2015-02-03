#include "nibble.h"

/*
 * Gedare Bloom
 *
 * Nibble Sort Programming Contest
 * Sort the 4-bit pieces of a 64-bit word with (unsigned) smaller values
 * towards the small end of the word.
 * Example: The nibble sort of 0xbadbeef is 0xfeedbba000000000.
 * The function you implement will perform this sorting operation on a buffer
 * of 1024 64-bit integers:
 * 	void nibble_sort(unsigned long *buf);
 */

static unsigned long nibble_sort_word(unsigned long arg) {
  int i, index;
  unsigned long rv = 0;
  unsigned char *bytes = (unsigned char *)&arg;
  int nibbles[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (i = 7; i >= 0; i--) {
    unsigned char top = bytes[i] >> 4;
    unsigned char bot = bytes[i] & 0xF;
    ++nibbles[top];
    ++nibbles[bot];
  }

  for (i = 15, index = 60; i > 0 && index >= 0; i--) {
    while (nibbles[i]) {
      --nibbles[i];
      rv += ((unsigned long)i << index);
      index -= 4;
    }
  }
  return rv;
}

void nibble_sort_bloom(unsigned long *buf) {
  int i;
  for (i = 0; i < 1024; i++) {
    buf[i] = nibble_sort_word(buf[i]);
  }
}
