#include <stdint.h>

#include "nibble.h"

static uint64_t table[256];
static uint64_t offsets[256];
static uint64_t table3[256 * 17];

__attribute__((constructor)) static void init_table() {
  for (int i = 0; i < 16; i++)
    for (int j = 0; j < 16; j++) {
      table[i * 16 + j] = (UINT64_C(1) << 4 * i) + (UINT64_C(1) << 4 * j);
      offsets[i * 16 + j] = (i + j) << 8;
      for (int k = 0; k < 17; k++)
        table3[i * 16 + j + k * 256] =
            ((i + j + k >= 16) ? 0 : (0x1111111111111111 << 4 * (i + j + k))) +
            ((j + k >= 16) ? 0 : (0x1111111111111111 << 4 * (j + k)));
    }
}

static uint64_t sort_word(uint64_t word) {
  if ((word << 4 | word >> 60) == word)
    return word;
  uint64_t counts = 0;
  for (int i = 0; i < 8; i++)
    counts += table[(word >> 8 * i) & 0xff];
  uint64_t output = 0;
  uint64_t offset = 0;
  for (int i = 0; i < 8; i++) {
    output += table3[((counts >> 8 * i) & 0xff) + offset];
    offset += offsets[(counts >> 8 * i) & 0xff];
  }
  return output;
}

void nibble_sort_jerome(uint64_t *buf) {
  for (int i = 0; i < 1024; i += 4) {
    buf[i] = sort_word(buf[i]);
    buf[i + 1] = sort_word(buf[i + 1]);
    buf[i + 2] = sort_word(buf[i + 2]);
    buf[i + 3] = sort_word(buf[i + 3]);
  }
}
