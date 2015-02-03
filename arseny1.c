#include "nibble.h"

#include <immintrin.h>

static __attribute__((always_inline)) inline __m256i
    _mm256_unpacklo_epi128(__m256i a, __m256i b) {
  return _mm256_permute2x128_si256(a, b, 0x20);
}

static __attribute__((always_inline)) inline __m256i
    _mm256_unpackhi_epi128(__m256i a, __m256i b) {
  return _mm256_permute2x128_si256(a, b, 0x31);
}

static __attribute__((always_inline)) inline void transpose_32x32(
    __m256i *x0, __m256i *x1, __m256i *x2, __m256i *x3, __m256i *x4,
    __m256i *x5, __m256i *x6, __m256i *x7, __m256i *x8, __m256i *x9,
    __m256i *xa, __m256i *xb, __m256i *xc, __m256i *xd, __m256i *xe,
    __m256i *xf, __m256i *xg, __m256i *xh, __m256i *xi, __m256i *xj,
    __m256i *xk, __m256i *xl, __m256i *xm, __m256i *xn, __m256i *xo,
    __m256i *xp, __m256i *xq, __m256i *xr, __m256i *xs, __m256i *xt,
    __m256i *xu, __m256i *xv) {
#define PAIR8(ra, rb, sa, sb)                                                  \
  __m256i ra = _mm256_unpacklo_epi8(sa, sb), rb = _mm256_unpackhi_epi8(sa, sb)
#define PAIR16(ra, rb, sa, sb)                                                 \
  __m256i ra = _mm256_unpacklo_epi16(sa, sb), rb = _mm256_unpackhi_epi16(sa, sb)
#define PAIR32(ra, rb, sa, sb)                                                 \
  __m256i ra = _mm256_unpacklo_epi32(sa, sb), rb = _mm256_unpackhi_epi32(sa, sb)
#define PAIR64(ra, rb, sa, sb)                                                 \
  __m256i ra = _mm256_unpacklo_epi64(sa, sb), rb = _mm256_unpackhi_epi64(sa, sb)
#define PAIR128(ra, rb, sa, sb)                                                \
  __m256i ra = _mm256_unpacklo_epi128(sa, sb),                                 \
          rb = _mm256_unpackhi_epi128(sa, sb)

  PAIR8(x01_0, x01_1, *x0, *x1);
  PAIR8(x23_0, x23_1, *x2, *x3);
  PAIR8(x45_0, x45_1, *x4, *x5);
  PAIR8(x67_0, x67_1, *x6, *x7);
  PAIR8(x89_0, x89_1, *x8, *x9);
  PAIR8(xab_0, xab_1, *xa, *xb);
  PAIR8(xcd_0, xcd_1, *xc, *xd);
  PAIR8(xef_0, xef_1, *xe, *xf);
  PAIR8(xgh_0, xgh_1, *xg, *xh);
  PAIR8(xij_0, xij_1, *xi, *xj);
  PAIR8(xkl_0, xkl_1, *xk, *xl);
  PAIR8(xmn_0, xmn_1, *xm, *xn);
  PAIR8(xop_0, xop_1, *xo, *xp);
  PAIR8(xqr_0, xqr_1, *xq, *xr);
  PAIR8(xst_0, xst_1, *xs, *xt);
  PAIR8(xuv_0, xuv_1, *xu, *xv);

  PAIR16(x0123_00, x0123_01, x01_0, x23_0);
  PAIR16(x0123_10, x0123_11, x01_1, x23_1);
  PAIR16(x4567_00, x4567_01, x45_0, x67_0);
  PAIR16(x4567_10, x4567_11, x45_1, x67_1);
  PAIR16(x89ab_00, x89ab_01, x89_0, xab_0);
  PAIR16(x89ab_10, x89ab_11, x89_1, xab_1);
  PAIR16(xcdef_00, xcdef_01, xcd_0, xef_0);
  PAIR16(xcdef_10, xcdef_11, xcd_1, xef_1);
  PAIR16(xghij_00, xghij_01, xgh_0, xij_0);
  PAIR16(xghij_10, xghij_11, xgh_1, xij_1);
  PAIR16(xklmn_00, xklmn_01, xkl_0, xmn_0);
  PAIR16(xklmn_10, xklmn_11, xkl_1, xmn_1);
  PAIR16(xopqr_00, xopqr_01, xop_0, xqr_0);
  PAIR16(xopqr_10, xopqr_11, xop_1, xqr_1);
  PAIR16(xstuv_00, xstuv_01, xst_0, xuv_0);
  PAIR16(xstuv_10, xstuv_11, xst_1, xuv_1);

  PAIR32(x01234567_000, x01234567_001, x0123_00, x4567_00);
  PAIR32(x01234567_010, x01234567_011, x0123_01, x4567_01);
  PAIR32(x01234567_100, x01234567_101, x0123_10, x4567_10);
  PAIR32(x01234567_110, x01234567_111, x0123_11, x4567_11);
  PAIR32(x89abcdef_000, x89abcdef_001, x89ab_00, xcdef_00);
  PAIR32(x89abcdef_010, x89abcdef_011, x89ab_01, xcdef_01);
  PAIR32(x89abcdef_100, x89abcdef_101, x89ab_10, xcdef_10);
  PAIR32(x89abcdef_110, x89abcdef_111, x89ab_11, xcdef_11);
  PAIR32(xghijklmn_000, xghijklmn_001, xghij_00, xklmn_00);
  PAIR32(xghijklmn_010, xghijklmn_011, xghij_01, xklmn_01);
  PAIR32(xghijklmn_100, xghijklmn_101, xghij_10, xklmn_10);
  PAIR32(xghijklmn_110, xghijklmn_111, xghij_11, xklmn_11);
  PAIR32(xopqrstuv_000, xopqrstuv_001, xopqr_00, xstuv_00);
  PAIR32(xopqrstuv_010, xopqrstuv_011, xopqr_01, xstuv_01);
  PAIR32(xopqrstuv_100, xopqrstuv_101, xopqr_10, xstuv_10);
  PAIR32(xopqrstuv_110, xopqrstuv_111, xopqr_11, xstuv_11);

  PAIR64(xr0_0000, xr0_0001, x01234567_000, x89abcdef_000);
  PAIR64(xr0_0010, xr0_0011, x01234567_001, x89abcdef_001);
  PAIR64(xr0_0100, xr0_0101, x01234567_010, x89abcdef_010);
  PAIR64(xr0_0110, xr0_0111, x01234567_011, x89abcdef_011);
  PAIR64(xr0_1000, xr0_1001, x01234567_100, x89abcdef_100);
  PAIR64(xr0_1010, xr0_1011, x01234567_101, x89abcdef_101);
  PAIR64(xr0_1100, xr0_1101, x01234567_110, x89abcdef_110);
  PAIR64(xr0_1110, xr0_1111, x01234567_111, x89abcdef_111);
  PAIR64(xr1_0000, xr1_0001, xghijklmn_000, xopqrstuv_000);
  PAIR64(xr1_0010, xr1_0011, xghijklmn_001, xopqrstuv_001);
  PAIR64(xr1_0100, xr1_0101, xghijklmn_010, xopqrstuv_010);
  PAIR64(xr1_0110, xr1_0111, xghijklmn_011, xopqrstuv_011);
  PAIR64(xr1_1000, xr1_1001, xghijklmn_100, xopqrstuv_100);
  PAIR64(xr1_1010, xr1_1011, xghijklmn_101, xopqrstuv_101);
  PAIR64(xr1_1100, xr1_1101, xghijklmn_110, xopqrstuv_110);
  PAIR64(xr1_1110, xr1_1111, xghijklmn_111, xopqrstuv_111);

  PAIR128(xr_00000, xr_10000, xr0_0000, xr1_0000);
  PAIR128(xr_00001, xr_10001, xr0_0001, xr1_0001);
  PAIR128(xr_00010, xr_10010, xr0_0010, xr1_0010);
  PAIR128(xr_00011, xr_10011, xr0_0011, xr1_0011);
  PAIR128(xr_00100, xr_10100, xr0_0100, xr1_0100);
  PAIR128(xr_00101, xr_10101, xr0_0101, xr1_0101);
  PAIR128(xr_00110, xr_10110, xr0_0110, xr1_0110);
  PAIR128(xr_00111, xr_10111, xr0_0111, xr1_0111);
  PAIR128(xr_01000, xr_11000, xr0_1000, xr1_1000);
  PAIR128(xr_01001, xr_11001, xr0_1001, xr1_1001);
  PAIR128(xr_01010, xr_11010, xr0_1010, xr1_1010);
  PAIR128(xr_01011, xr_11011, xr0_1011, xr1_1011);
  PAIR128(xr_01100, xr_11100, xr0_1100, xr1_1100);
  PAIR128(xr_01101, xr_11101, xr0_1101, xr1_1101);
  PAIR128(xr_01110, xr_11110, xr0_1110, xr1_1110);
  PAIR128(xr_01111, xr_11111, xr0_1111, xr1_1111);

  *x0 = xr_00000;
  *x1 = xr_00001;
  *x2 = xr_00010;
  *x3 = xr_00011;
  *x4 = xr_00100;
  *x5 = xr_00101;
  *x6 = xr_00110;
  *x7 = xr_00111;
  *x8 = xr_01000;
  *x9 = xr_01001;
  *xa = xr_01010;
  *xb = xr_01011;
  *xc = xr_01100;
  *xd = xr_01101;
  *xe = xr_01110;
  *xf = xr_01111;
  *xg = xr_10000;
  *xh = xr_10001;
  *xi = xr_10010;
  *xj = xr_10011;
  *xk = xr_10100;
  *xl = xr_10101;
  *xm = xr_10110;
  *xn = xr_10111;
  *xo = xr_11000;
  *xp = xr_11001;
  *xq = xr_11010;
  *xr = xr_11011;
  *xs = xr_11100;
  *xt = xr_11101;
  *xu = xr_11110;
  *xv = xr_11111;

#undef PAIR8
#undef PAIR16
#undef PAIR32
#undef PAIR64
#undef PAIR128
}

void nibble_sort_arseny1(unsigned long *buf) {
  for (unsigned wit = 0; wit < 1024; wit += 128) {
#define LOAD(i) __m256i reg##i = _mm256_load_si256((__m256i *)&buf[wit + i * 4])

    LOAD(0);
    LOAD(1);
    LOAD(2);
    LOAD(3);
    LOAD(4);
    LOAD(5);
    LOAD(6);
    LOAD(7);
    LOAD(8);
    LOAD(9);
    LOAD(10);
    LOAD(11);
    LOAD(12);
    LOAD(13);
    LOAD(14);
    LOAD(15);
    LOAD(16);
    LOAD(17);
    LOAD(18);
    LOAD(19);
    LOAD(20);
    LOAD(21);
    LOAD(22);
    LOAD(23);
    LOAD(24);
    LOAD(25);
    LOAD(26);
    LOAD(27);
    LOAD(28);
    LOAD(29);
    LOAD(30);
    LOAD(31);

#undef LOAD

    transpose_32x32(&reg0, &reg1, &reg2, &reg3, &reg4, &reg5, &reg6, &reg7,
                    &reg8, &reg9, &reg10, &reg11, &reg12, &reg13, &reg14,
                    &reg15, &reg16, &reg17, &reg18, &reg19, &reg20, &reg21,
                    &reg22, &reg23, &reg24, &reg25, &reg26, &reg27, &reg28,
                    &reg29, &reg30, &reg31);

    __m256i sort0, sort1, sort2, sort3, sort4, sort5, sort6, sort7, sort8,
        sort9, sort10, sort11, sort12, sort13, sort14, sort15;

    __m256i tempi, tempj;

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
  ra = _mm256_and_si256(_mm256_srli_epi16(src, 4), _mm256_set1_epi8(15));      \
  rb = _mm256_and_si256(src, _mm256_set1_epi8(15))

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

    CONVERT(UNPACK, reg8, reg9, reg10, reg11, reg12, reg13, reg14, reg15);
    SORT();
    CONVERT(PACK, reg8, reg9, reg10, reg11, reg12, reg13, reg14, reg15);

    CONVERT(UNPACK, reg16, reg17, reg18, reg19, reg20, reg21, reg22, reg23);
    SORT();
    CONVERT(PACK, reg16, reg17, reg18, reg19, reg20, reg21, reg22, reg23);

    CONVERT(UNPACK, reg24, reg25, reg26, reg27, reg28, reg29, reg30, reg31);
    SORT();
    CONVERT(PACK, reg24, reg25, reg26, reg27, reg28, reg29, reg30, reg31);

#undef CONVERT
#undef PACK
#undef UNPACK
#undef SORT
#undef SWAP

    transpose_32x32(&reg0, &reg1, &reg2, &reg3, &reg4, &reg5, &reg6, &reg7,
                    &reg8, &reg9, &reg10, &reg11, &reg12, &reg13, &reg14,
                    &reg15, &reg16, &reg17, &reg18, &reg19, &reg20, &reg21,
                    &reg22, &reg23, &reg24, &reg25, &reg26, &reg27, &reg28,
                    &reg29, &reg30, &reg31);

#define SAVE(i) _mm256_store_si256((__m256i *)&buf[wit + i * 4], reg##i)

    SAVE(0);
    SAVE(1);
    SAVE(2);
    SAVE(3);
    SAVE(4);
    SAVE(5);
    SAVE(6);
    SAVE(7);
    SAVE(8);
    SAVE(9);
    SAVE(10);
    SAVE(11);
    SAVE(12);
    SAVE(13);
    SAVE(14);
    SAVE(15);
    SAVE(16);
    SAVE(17);
    SAVE(18);
    SAVE(19);
    SAVE(20);
    SAVE(21);
    SAVE(22);
    SAVE(23);
    SAVE(24);
    SAVE(25);
    SAVE(26);
    SAVE(27);
    SAVE(28);
    SAVE(29);
    SAVE(30);
    SAVE(31);

#undef SAVE
  }
}
