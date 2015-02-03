#include "nibble.h"

// supports SSE SSE2 SSE3 SSE4.1 SSE4.2
#include <tmmintrin.h> // SSE3
#include <smmintrin.h> // SSE4

// PEXTRB extract byte;   _mm_extract_epi8
// PSHUFB shuffle byte;   _mm_shuffle_epi8

// shuffle extend pattern
static __m128i vm, vnib, vbl, vlo, vhi;

typedef struct {
  unsigned int p0 : 4;
  unsigned int p1 : 4;
  unsigned int p2 : 4;
  unsigned int p3 : 4;
  unsigned int p4 : 4;
  unsigned int p5 : 4;
  unsigned int p6 : 4;
  unsigned int p7 : 4;
  unsigned int p8 : 4;
  unsigned int p9 : 4;
  unsigned int pa : 4;
  unsigned int pb : 4;
  unsigned int pc : 4;
  unsigned int pd : 4;
  unsigned int pe : 4;
  unsigned int pf : 4;
} m64_4;

static __m128i word_to_nibbles(uint64_t word) {
  m64_4 n = *(m64_4 *)&word;
  __m128i w128 = _mm_set_epi8(n.pf, n.pe, n.pd, n.pc, n.pb, n.pa, n.p9, n.p8,
                              n.p7, n.p6, n.p5, n.p4, n.p3, n.p2, n.p1, n.p0);

  return w128;
}

static uint64_t nibbles_to_word(__m128i vec) {
  __m128i lows;
  uint64_t *loa;

  lows = _mm_shuffle_epi8(vec, vlo);

  loa = (uint64_t *)&lows;

  return loa[0] | (loa[1] << 4);
}

static void cNs(uint8_t *nib, uint8_t i, uint8_t j) {
  uint8_t ihi, ilo, tmp;

  ihi = i < j ? j : i;
  ilo = i < j ? i : j;

  if (nib[ihi] < nib[ilo]) {
    tmp = nib[ihi];
    nib[ihi] = nib[ilo];
    nib[ilo] = tmp;
  }
}

static uint64_t nibble_sort_word(uint64_t arg) {
  __m128i n128 = word_to_nibbles(arg);
  uint8_t *nib = (uint8_t *)&n128;

  // green's sort
  cNs(nib, 0x0, 0x1);
  cNs(nib, 0x2, 0x3);
  cNs(nib, 0x4, 0x5);
  cNs(nib, 0x6, 0x7);
  cNs(nib, 0x8, 0x9);
  cNs(nib, 0xa, 0xb);
  cNs(nib, 0xc, 0xd);
  cNs(nib, 0xe, 0xf);

  cNs(nib, 0x0, 0x2);
  cNs(nib, 0x1, 0x3);
  cNs(nib, 0x4, 0x6);
  cNs(nib, 0x5, 0x7);
  cNs(nib, 0x8, 0xa);
  cNs(nib, 0x9, 0xb);
  cNs(nib, 0xc, 0xe);
  cNs(nib, 0xd, 0xf);

  cNs(nib, 0x0, 0x4);
  cNs(nib, 0x1, 0x5);
  cNs(nib, 0x2, 0x6);
  cNs(nib, 0x3, 0x7);
  cNs(nib, 0x8, 0xc);
  cNs(nib, 0x9, 0xd);
  cNs(nib, 0xa, 0xe);
  cNs(nib, 0xb, 0xf);

  cNs(nib, 0x0, 0x8);
  cNs(nib, 0x1, 0x9);
  cNs(nib, 0x2, 0xa);
  cNs(nib, 0x3, 0xb);
  cNs(nib, 0x4, 0xc);
  cNs(nib, 0x5, 0xd);
  cNs(nib, 0x6, 0xe);
  cNs(nib, 0x7, 0xf);

  cNs(nib, 0x5, 0xa);
  cNs(nib, 0x6, 0x9);
  cNs(nib, 0x3, 0xc);
  cNs(nib, 0xd, 0xe);
  cNs(nib, 0x7, 0xb);
  cNs(nib, 0x1, 0x2);
  cNs(nib, 0x4, 0x8);

  cNs(nib, 0x1, 0x4);
  cNs(nib, 0x7, 0xd);
  cNs(nib, 0x2, 0x8);
  cNs(nib, 0xb, 0xe);

  cNs(nib, 0x2, 0x4);
  cNs(nib, 0x5, 0x6);
  cNs(nib, 0x9, 0xa);
  cNs(nib, 0xb, 0xd);
  cNs(nib, 0x3, 0x8);
  cNs(nib, 0x7, 0xc);

  cNs(nib, 0x6, 0x8);
  cNs(nib, 0xa, 0xc);
  cNs(nib, 0x3, 0x5);
  cNs(nib, 0x7, 0x9);

  cNs(nib, 0x3, 0x4);
  cNs(nib, 0x5, 0x6);
  cNs(nib, 0x7, 0x8);
  cNs(nib, 0x9, 0xa);
  cNs(nib, 0xb, 0xc);

  cNs(nib, 0x6, 0x7);
  cNs(nib, 0x8, 0x9);

  return nibbles_to_word(*(__m128i *)nib);
}

void nibble_sort_mikael(uint64_t *buf) {
  vm = _mm_setr_epi8(0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7);
  vnib = _mm_setr_epi8(0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
                       0xf, 0xf, 0xf, 0xf, 0xf);
  vbl = _mm_setr_epi8(0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80,
                      0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00);
  vlo = _mm_setr_epi8(0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15);
  vhi = _mm_setr_epi8(1, 3, 5, 7, 9, 11, 13, 15, 0, 2, 4, 6, 8, 10, 12, 14);

  for (int i = 0; i < 1024; i++)
    buf[i] = nibble_sort_word(buf[i]);
}
