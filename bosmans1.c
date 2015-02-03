#include "smmintrin.h"

#include "nibble.h"

static void nibble_sort_word_mkbosmans_sse(unsigned long *arg) {
  /* Load the nibbles of arg into the bytes of arg16 */
  __m128i arg_in = _mm_loadl_epi64((void *)arg);
  __m128i arg16 =
      _mm_and_si128(_mm_unpacklo_epi8(arg_in, _mm_srli_epi64(arg_in, 4)),
                    _mm_set1_epi8(0x0f));

  /* For each nibble value, calculate the vector with 0x04 set at the locations
   * of that value in arg16 */
  __m128i loc_0 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000000, 0x0000000000000004), arg16);
  __m128i loc_1 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000000, 0x0000000000000400), arg16);
  __m128i loc_2 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000000, 0x0000000000040000), arg16);
  __m128i loc_3 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000000, 0x0000000004000000), arg16);
  __m128i loc_4 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000000, 0x0000000400000000), arg16);
  __m128i loc_5 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000000, 0x0000040000000000), arg16);
  __m128i loc_6 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000000, 0x0004000000000000), arg16);
  __m128i loc_7 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000000, 0x0400000000000000), arg16);
  __m128i loc_8 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000004, 0x0000000000000000), arg16);
  __m128i loc_9 = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000000400, 0x0000000000000000), arg16);
  __m128i loc_a = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000000040000, 0x0000000000000000), arg16);
  __m128i loc_b = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000004000000, 0x0000000000000000), arg16);
  __m128i loc_c = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000000400000000, 0x0000000000000000), arg16);
  __m128i loc_d = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0000040000000000, 0x0000000000000000), arg16);
  __m128i loc_e = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0004000000000000, 0x0000000000000000), arg16);
  __m128i loc_f = _mm_shuffle_epi8(
      _mm_set_epi64x(0x0400000000000000, 0x0000000000000000), arg16);

/* Use horizontal adds to calculate the total count of each nibble */
#define hadd(a, b, w)                                                          \
  _mm_add_epi8(_mm_unpacklo_epi##w(a, b), _mm_unpackhi_epi##w(a, b))
  __m128i sum1_0 = hadd(loc_0, loc_1, 8);
  __m128i sum1_1 = hadd(loc_2, loc_3, 8);
  __m128i sum1_2 = hadd(loc_4, loc_5, 8);
  __m128i sum1_3 = hadd(loc_6, loc_7, 8);
  __m128i sum1_4 = hadd(loc_8, loc_9, 8);
  __m128i sum1_5 = hadd(loc_a, loc_b, 8);
  __m128i sum1_6 = hadd(loc_c, loc_d, 8);
  __m128i sum1_7 = hadd(loc_e, loc_f, 8);

  __m128i sum2_0 = hadd(sum1_0, sum1_1, 16);
  __m128i sum2_1 = hadd(sum1_2, sum1_3, 16);
  __m128i sum2_2 = hadd(sum1_4, sum1_5, 16);
  __m128i sum2_3 = hadd(sum1_6, sum1_7, 16);

  __m128i sum3_0 = hadd(sum2_0, sum2_1, 32);
  __m128i sum3_1 = hadd(sum2_2, sum2_3, 32);

  __m128i count = hadd(sum3_0, sum3_1, 64);

  /* Use the nibble count vector to build the sorted result */
  unsigned long res = 0;
  unsigned long mask_l = 0xffffffffffffffff, mask_r;
  for (unsigned long n = 0; n < 16; n++) {
    mask_r = mask_l;
    mask_l <<= _mm_extract_epi8(count, n);
    res |= (0x1111111111111111 * n) & ~mask_l & mask_r;
  }
  *arg = res;
}

void nibble_sort_bosmans1(unsigned long *buf) {
  for (int i = 0; i < 1024; i++)
    nibble_sort_word_mkbosmans_sse(buf++);
}
