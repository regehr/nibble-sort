#include "nibble.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <emmintrin.h>
#include <smmintrin.h>

/*
  5.8.5 Packed Shuffle Bytes
  PSHUFB Permutes each byte in place, according to
  a shuffle control mask. The least significant three or
  four bits of each shuffle control byte of the control mask form the shuffle
  index. The shuffle
  mask is unaffected. If the most significant bit
  (bit 7) of a shuffle control byte is set, the
  constant zero is written in the result byte
*/

// typedef unsigned char uchar

// extern const int BUFSIZE;

/* compile with:
 gcc -std=gnu99 -march=native -fbranch-probabilities -ftracer
 -fprefetch-loop-arrays -O3
*/

void nibble_sort_scotty(unsigned long *buf);

static inline unsigned long create_mask(char amount, char type, char location) {
  unsigned long all = type;
  int i;

  switch (amount) {
  case 0:
    return 0;
  default:
    for (i = 0; i < amount - 1; i++) {
      all = ((all << 4) | type);
    }
    return all << location;
  }
}

static inline unsigned long write_new(__m128i read) {
  unsigned long write = 0;
  int i;
  unsigned int where = 64;
  unsigned int pulled;
  /* ignore zeros saves on loop iterations */

  pulled = _mm_extract_epi8(read, 15);
  where = where - (pulled * 4);
  write |= create_mask(pulled, 0xF, where);

  pulled = _mm_extract_epi8(read, 14);
  where = where - (pulled * 4);
  write |= create_mask(pulled, 0xE, where);

  pulled = _mm_extract_epi8(read, 13);
  where = where - (pulled * 4);
  write |= create_mask(pulled, 0xD, where);

  pulled = _mm_extract_epi8(read, 12);
  where = where - (pulled * 4);
  write |= create_mask(pulled, 0xC, where);

  pulled = _mm_extract_epi8(read, 11);
  where = where - (pulled * 4);
  write |= create_mask(pulled, 0xB, where);

  pulled = _mm_extract_epi8(read, 10);
  where = where - (pulled * 4);
  write |= create_mask(pulled, 0xA, where);

  pulled = _mm_extract_epi8(read, 9);
  where = where - (pulled * 4);
  write |= create_mask(pulled, 9, where);

  pulled = _mm_extract_epi8(read, 8);
  where = where - (pulled * 4);
  write |= create_mask(pulled, 8, where);

  pulled = _mm_extract_epi8(read, 7);
  where = where - (pulled * 4);
  write |= create_mask(pulled, 7, where);

  pulled = _mm_extract_epi8(read, 6);
  where = where - (pulled * 4);
  ;
  write |= create_mask(pulled, 6, where);

  pulled = _mm_extract_epi8(read, 5);
  where = where - (pulled * 4);
  ;
  write |= create_mask(pulled, 5, where);

  pulled = _mm_extract_epi8(read, 4);
  where = where - (pulled * 4);
  ;
  write |= create_mask(pulled, 4, where);

  pulled = _mm_extract_epi8(read, 3);
  where = where - (pulled * 4);
  ;
  write |= create_mask(pulled, 3, where);

  pulled = _mm_extract_epi8(read, 2);
  where = where - (pulled * 4);
  ;
  write |= create_mask(pulled, 2, where);

  pulled = _mm_extract_epi8(read, 1);
  where = where - (pulled * 4);
  ;
  write |= create_mask(pulled, 1, where);

  return write;
}

void nibble_sort_scotty(unsigned long *temp) {
  __builtin_prefetch(temp, 1, 3);
  unsigned long sorted;
  __m128i all = {0};
  __m128i zero = {0};

  int i, j;
  int word_holder;
  unsigned long *buf = temp;
  /* NOTES:
     epi32 extract so one can preserver the "higher" bits of 'all'
     if we didnt we'd be slamming 0's into all ruining the count
  */
  for (j = 0; j < 1024; j++) {
    for (i = 0; i < 16; i++) {
      /* todo mm extract the byte then nibble shift */
      if (*buf == 0)
        break;

      switch (((*buf) & 0xf)) {
      case 0:
        // word_holder = _mm_extract_epi32(all, 0);
        // all = _mm_insert_epi32(all, word_holder + 0x1, 0);
        break;
      case 1:
        word_holder = _mm_extract_epi32(all, 0);
        all = _mm_insert_epi32(all, (word_holder + 0x100), 0);
        break;
      case 2:
        word_holder = _mm_extract_epi32(all, 0);
        all = _mm_insert_epi32(all, (word_holder + 0x10000), 0);
        break;
      case 3:
        word_holder = _mm_extract_epi32(all, 0);
        all = _mm_insert_epi32(all, (word_holder + 0x1000000), 0);
        break;
      case 4:
        word_holder = _mm_extract_epi32(all, 1);
        all = _mm_insert_epi32(all, (word_holder + 0x1), 1);
        break;
      case 5:
        word_holder = _mm_extract_epi32(all, 1);
        all = _mm_insert_epi32(all, (word_holder + 0x100), 1);
        break;
      case 6:
        word_holder = _mm_extract_epi32(all, 1);
        all = _mm_insert_epi32(all, (word_holder + 0x10000), 1);
        break;
      case 7:
        word_holder = _mm_extract_epi32(all, 1);
        all = _mm_insert_epi32(all, (word_holder + 0x1000000), 1);
        break;
      case 8:
        word_holder = _mm_extract_epi32(all, 2);
        all = _mm_insert_epi32(all, (word_holder + 0x1), 2);
        break;
      case 9:
        word_holder = _mm_extract_epi32(all, 2);
        all = _mm_insert_epi32(all, (word_holder + 0x100), 2);
        break;
      case 0xA:
        word_holder = _mm_extract_epi32(all, 2);
        all = _mm_insert_epi32(all, (word_holder + 0x10000), 2);
        break;
      case 0xB:
        word_holder = _mm_extract_epi32(all, 2);
        all = _mm_insert_epi32(all, (word_holder + 0x1000000), 2);
        break;
      case 0xC:
        word_holder = _mm_extract_epi32(all, 3);
        all = _mm_insert_epi32(all, (word_holder + 0x1), 3);
        break;
      case 0xD:
        word_holder = _mm_extract_epi32(all, 3);
        all = _mm_insert_epi32(all, (word_holder + 0x100), 3);
        break;
      case 0xE:
        word_holder = _mm_extract_epi32(all, 3);
        all = _mm_insert_epi32(all, (word_holder + 0x10000), 3);
        break;
      case 0xF:
        word_holder = _mm_extract_epi32(all, 3);
        all = _mm_insert_epi32(all, (word_holder + 0x1000000), 3);
        break;
      default:
        __builtin_abort();
      }
      *buf >>= 4;
    }
    __builtin_prefetch((buf + 1), 1, 3);
    *buf = write_new(all);
    all = zero;
    buf++;
  }
}

/*
int main(void)
{
        //	unsigned long test = 0xc67b1f9a06cac113;
        unsigned long test = 0x728dc8ba40781809;//0xfbbbc9741902c44c;// at 0 to
sort to fcccbbb997444210
        nibble_sort_scotty(&test);
        printf("%lx\n", test);

}


*/
