#include <immintrin.h>

__attribute__((always_inline)) inline void transpose_32x8_block(
    __m256i *x0, __m256i *x1, __m256i *x2, __m256i *x3, __m256i *x4,
    __m256i *x5, __m256i *x6, __m256i *x7) {
#define PAIR8(ra, rb, sa, sb)                                                  \
  __m256i ra = _mm256_unpacklo_epi8(sa, sb), rb = _mm256_unpackhi_epi8(sa, sb)
#define PAIR16(ra, rb, sa, sb)                                                 \
  __m256i ra = _mm256_unpacklo_epi16(sa, sb), rb = _mm256_unpackhi_epi16(sa, sb)
#define PAIR32(ra, rb, sa, sb)                                                 \
  __m256i ra = _mm256_unpacklo_epi32(sa, sb), rb = _mm256_unpackhi_epi32(sa, sb)
#define PAIR64(ra, rb, sa, sb)                                                 \
  __m256i ra = _mm256_unpacklo_epi64(sa, sb), rb = _mm256_unpackhi_epi64(sa, sb)

  PAIR8(x01_0, x01_1, *x0, *x1);
  PAIR8(x23_0, x23_1, *x2, *x3);
  PAIR8(x45_0, x45_1, *x4, *x5);
  PAIR8(x67_0, x67_1, *x6, *x7);

  PAIR16(x0123_00, x0123_01, x01_0, x23_0);
  PAIR16(x0123_10, x0123_11, x01_1, x23_1);
  PAIR16(x4567_00, x4567_01, x45_0, x67_0);
  PAIR16(x4567_10, x4567_11, x45_1, x67_1);

  PAIR32(x01234567_000, x01234567_001, x0123_00, x4567_00);
  PAIR32(x01234567_010, x01234567_011, x0123_01, x4567_01);
  PAIR32(x01234567_100, x01234567_101, x0123_10, x4567_10);
  PAIR32(x01234567_110, x01234567_111, x0123_11, x4567_11);

  PAIR64(xr_000, xr_001, x01234567_000, x01234567_100);
  PAIR64(xr_010, xr_011, x01234567_001, x01234567_101);
  PAIR64(xr_100, xr_101, x01234567_010, x01234567_110);
  PAIR64(xr_110, xr_111, x01234567_011, x01234567_111);

  *x0 = xr_000;
  *x1 = xr_001;
  *x2 = xr_010;
  *x3 = xr_011;
  *x4 = xr_100;
  *x5 = xr_101;
  *x6 = xr_110;
  *x7 = xr_111;

#undef PAIR8
#undef PAIR16
#undef PAIR32
}

void nibble_sort_arseny2(unsigned long *buf) {
  // this is to trick gcc into storing <0xf> in a register in the inner loop
  // if gcc knows that it's a constant it loads it using rip-relative addressing
  // inside the inner loop (for every vpand instruction!)
  // this makes the code slower (1250 ns vs 1190 ns)
  static __m256i gnibble;
  if (buf)
    gnibble = _mm256_set1_epi8(15);

  for (unsigned wit = 0; wit < 1024; wit += 32) {
#define LOAD(i) __m256i reg##i = _mm256_load_si256((__m256i *)&buf[wit + i * 4])

    LOAD(0);
    LOAD(1);
    LOAD(2);
    LOAD(3);
    LOAD(4);
    LOAD(5);
    LOAD(6);
    LOAD(7);

#undef LOAD

    transpose_32x8_block(&reg0, &reg1, &reg2, &reg3, &reg4, &reg5, &reg6,
                         &reg7);

    __m256i sort0, sort1, sort2, sort3, sort4, sort5, sort6, sort7, sort8,
        sort9, sort10, sort11, sort12, sort13, sort14, sort15;

    __m256i tempi, tempj;

    // this is a constant (but see the comment above gnibble definition)
    __m256i nibble = gnibble;

#define SWAP(i, j)                                                             \
  tempi = _mm256_min_epi8(sort##i, sort##j);                                   \
  tempj = _mm256_max_epi8(sort##i, sort##j);                                   \
  sort##i = tempi;                                                             \
  sort##j = tempj

#define SORT()                                                                 \
  SWAP(0, 1);                                                                  \
  SWAP(2, 3);                                                                  \
  SWAP(4, 5);                                                                  \
  SWAP(6, 7);                                                                  \
  SWAP(8, 9);                                                                  \
  SWAP(10, 11);                                                                \
  SWAP(12, 13);                                                                \
  SWAP(14, 15);                                                                \
  SWAP(0, 2);                                                                  \
  SWAP(4, 6);                                                                  \
  SWAP(8, 10);                                                                 \
  SWAP(12, 14);                                                                \
  SWAP(1, 3);                                                                  \
  SWAP(5, 7);                                                                  \
  SWAP(9, 11);                                                                 \
  SWAP(13, 15);                                                                \
  SWAP(0, 4);                                                                  \
  SWAP(8, 12);                                                                 \
  SWAP(1, 5);                                                                  \
  SWAP(9, 13);                                                                 \
  SWAP(2, 6);                                                                  \
  SWAP(10, 14);                                                                \
  SWAP(3, 7);                                                                  \
  SWAP(11, 15);                                                                \
  SWAP(0, 8);                                                                  \
  SWAP(1, 9);                                                                  \
  SWAP(2, 10);                                                                 \
  SWAP(3, 11);                                                                 \
  SWAP(4, 12);                                                                 \
  SWAP(5, 13);                                                                 \
  SWAP(6, 14);                                                                 \
  SWAP(7, 15);                                                                 \
  SWAP(5, 10);                                                                 \
  SWAP(6, 9);                                                                  \
  SWAP(3, 12);                                                                 \
  SWAP(13, 14);                                                                \
  SWAP(7, 11);                                                                 \
  SWAP(1, 2);                                                                  \
  SWAP(4, 8);                                                                  \
  SWAP(1, 4);                                                                  \
  SWAP(7, 13);                                                                 \
  SWAP(2, 8);                                                                  \
  SWAP(11, 14);                                                                \
  SWAP(2, 4);                                                                  \
  SWAP(5, 6);                                                                  \
  SWAP(9, 10);                                                                 \
  SWAP(11, 13);                                                                \
  SWAP(3, 8);                                                                  \
  SWAP(7, 12);                                                                 \
  SWAP(6, 8);                                                                  \
  SWAP(10, 12);                                                                \
  SWAP(3, 5);                                                                  \
  SWAP(7, 9);                                                                  \
  SWAP(3, 4);                                                                  \
  SWAP(5, 6);                                                                  \
  SWAP(7, 8);                                                                  \
  SWAP(9, 10);                                                                 \
  SWAP(11, 12);                                                                \
  SWAP(6, 7);                                                                  \
  SWAP(8, 9)

#define UNPACK(ra, rb, src)                                                    \
  ra = _mm256_and_si256(_mm256_srli_epi16(src, 4), nibble);                    \
  rb = _mm256_and_si256(src, nibble)

#define PACK(ra, rb, tgt) tgt = _mm256_or_si256(ra, _mm256_slli_epi16(rb, 4))

#define CONVERT(P, r0, r1, r2, r3, r4, r5, r6, r7)                             \
  P(sort0, sort1, r0);                                                         \
  P(sort2, sort3, r1);                                                         \
  P(sort4, sort5, r2);                                                         \
  P(sort6, sort7, r3);                                                         \
  P(sort8, sort9, r4);                                                         \
  P(sort10, sort11, r5);                                                       \
  P(sort12, sort13, r6);                                                       \
  P(sort14, sort15, r7)

    CONVERT(UNPACK, reg0, reg1, reg2, reg3, reg4, reg5, reg6, reg7);
    SORT();
    CONVERT(PACK, reg0, reg1, reg2, reg3, reg4, reg5, reg6, reg7);

#undef CONVERT
#undef PACK
#undef UNPACK
#undef SORT
#undef SWAP

    transpose_32x8_block(&reg0, &reg1, &reg2, &reg3, &reg4, &reg5, &reg6,
                         &reg7);

#define SAVE(i) _mm256_store_si256((__m256i *)&buf[wit + i * 4], reg##i)

    SAVE(0);
    SAVE(1);
    SAVE(2);
    SAVE(3);
    SAVE(4);
    SAVE(5);
    SAVE(6);
    SAVE(7);

#undef SAVE
  }
}