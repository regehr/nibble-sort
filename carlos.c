// -msse4.1

#include "nibble.h"

#include <emmintrin.h>
#include <mmintrin.h>
#include <pmmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>

#include <stdio.h>
#include <stdint.h>

/******************************************************************************/
// http://stackoverflow.com/questions/14733761/printf-formatting-for-hex

static int is_debugging = 0;

static void print_4way_hex_256(__m256i v) {
  uint64_t buf[4] __attribute__((aligned(32)));
  _mm256_store_si256((__m256i *)buf, v);
  printf("%#018llx %#018llx %#018llx %#018llx\n",
         (unsigned long long int)buf[0], (unsigned long long int)buf[1],
         (unsigned long long int)buf[2], (unsigned long long int)buf[3]);
}

static void print_4way_dec_256(__m256i v) {
  uint64_t buf[4] __attribute__((aligned(32)));
  _mm256_store_si256((__m256i *)buf, v);
  printf("% 4lld % 4lld % 4lld % 4lld\n", (long long int)buf[0],
         (long long int)buf[1], (long long int)buf[2], (long long int)buf[3]);
}

#define DEBUG_V256(x)                                                          \
  do {                                                                         \
    if (is_debugging) {                                                        \
      printf(#x ":\n");                                                        \
      printf("  ");                                                            \
      print_4way_hex_256(x);                                                   \
    }                                                                          \
  } while (0);

#define DEBUG_D256(x)                                                          \
  do {                                                                         \
    if (is_debugging) {                                                        \
      printf(#x ":\n");                                                        \
      printf("  ");                                                            \
      print_4way_dec_256(x);                                                   \
    }                                                                          \
  } while (0);

/******************************************************************************/

static __m256i mul_64bit_64bit_small(__m256i a, __m256i b) {
  __m256i r1 = _mm256_mul_epi32(a, b),
          r2 = _mm256_mul_epi32(_mm256_srli_epi64(a, 32), b);
  __m256i result = _mm256_add_epi64(
      _mm256_and_si256(r1, _mm256_set1_epi64x(0x00000000ffffffffULL)),
      _mm256_slli_epi64(r2, 32));
  return result;
}

/******************************************************************************/

// in the comments, we'll use "v" to denote the data-parallel word
// inside the 4-way array

// 4-way nibble counting; returns a 64-bit number with the 4-bit counts
// in the nibble positions
static __m256i count_nibbles(__m256i array) {
  __m256i result = _mm256_set1_epi64x(0), this_count, this_nibble;
  __m256i this_position;
  int i;

/* DEBUG_V256(array); */
#pragma unroll
  for (i = 0; i < 16; ++i) {
    // this_position = 4 * i;
    this_position = _mm256_set1_epi64x(4 * i);
    // this_nibble = (v >> this_position) & 15;
    this_nibble = _mm256_srlv_epi64(array, this_position);
    this_nibble = _mm256_and_si256(this_nibble, _mm256_set1_epi64x(15));
    /* DEBUG_V256(this_nibble); */
    // this_count = 1 << (4 * this_nibble)
    this_count = _mm256_sllv_epi64(
        _mm256_set1_epi64x(1),
        mul_64bit_64bit_small(this_nibble, _mm256_set1_epi64x(4)));
    /* DEBUG_V256(this_count); */
    // result += count
    result = _mm256_add_epi64(result, this_count);
    /* DEBUG_V256(result); */
  }
  /* DEBUG_V256(result); */
  return result;
}

// 4-way data-parallel, bitwise data-parallel, scan-summation for nibbles.
//
// WARNING! This scan_left stops one operation short. The reason for this
// is that we know nibbles cannot store counts up to 16, but can up to 15.
//
// in addition, the partial scan_left representation is easy to use
// to reconstruct what we need further down the line.
//
static __m256i scan_left(__m256i array) {
  __m256i a;

  /* DEBUG_V256(array); */
  array = _mm256_add_epi64(array, _mm256_slli_epi64(array, 4 * 8));
  array = _mm256_add_epi64(array, _mm256_slli_epi64(array, 4 * 4));
  array = _mm256_add_epi64(array, _mm256_slli_epi64(array, 4 * 2));

  // we can't make this last one, because nibbles only store counts up to 15,
  // and one position will always have too many
  // array = _mm256_add_epi64(array, _mm256_slli_epi64(array, 4 * 1));

  /* DEBUG_V256(array); */
  return array;
}

// unghhh npot just to get a sentinel for the bad corner case...
static int64_t repl[17] = {
    0x0000000000000000ULL, 0x0000000000000001ULL, 0x0000000000000011ULL,
    0x0000000000000111ULL, 0x0000000000001111ULL, 0x0000000000011111ULL,
    0x0000000000111111ULL, 0x0000000001111111ULL, 0x0000000011111111ULL,
    0x0000000111111111ULL, 0x0000001111111111ULL, 0x0000011111111111ULL,
    0x0000111111111111ULL, 0x0001111111111111ULL, 0x0011111111111111ULL,
    0x0111111111111111ULL, 0x1111111111111111ULL};

// NB the funky representation of position_array!!
static __m256i insertion(__m256i count_array, __m256i position_array) {
  __m256i result = _mm256_set1_epi64x(0), shift_amount_1, shift_amount_2,
          left_shift_amount, which_digit, count_amount, pattern;
  int i;

/* DEBUG_V256(count_array); */
/* DEBUG_V256(position_array); */

// we can skip 0 because we always multiply pattern by i
//
// we can skip i==16 because that's only one situation, and that'll be handled
// externally
// (and it breaks all of the count-scan-insert code, too)
#pragma unroll
  for (i = 1; i < 16; ++i) {
    /* if (is_debugging) printf("digit %d\n", i); */
    // this_position = 4 * i;
    // shift_amount = (position_v >> this_position) & 15
    shift_amount_1 = _mm256_srli_epi64(position_array, 4 * i);
    shift_amount_1 = _mm256_and_si256(shift_amount_1, _mm256_set1_epi64x(15));

    shift_amount_2 = _mm256_srli_epi64(position_array, 4 * (i - 1));
    shift_amount_2 = _mm256_and_si256(shift_amount_2, _mm256_set1_epi64x(15));

    // count = (count_v >> this_position) & 15
    count_amount = _mm256_srli_epi64(count_array, 4 * i);
    count_amount = _mm256_and_si256(count_amount, _mm256_set1_epi64x(15));
    /* DEBUG_D256(count_amount); */

    which_digit = _mm256_add_epi64(shift_amount_1, shift_amount_2);
    /* DEBUG_D256(which_digit); */

    /* left_shift_amount = _mm256_sub_epi64(_mm256_set1_epi64x(16),
     * which_digit); */
    /* DEBUG_D256(left_shift_amount); */

    // when shit overflows, which_digit == 0 and count_amount > 0
    // let's fix that shit.
    left_shift_amount = _mm256_slli_epi64(
        _mm256_add_epi64(
            _mm256_sub_epi64(which_digit, count_amount),
            _mm256_and_si256(
                _mm256_cmpeq_epi64(which_digit, _mm256_set1_epi64x(0)),
                _mm256_set1_epi64x(16))),
        2);

    pattern =
        _mm256_i64gather_epi64((const long long int *)repl, count_amount, 8);
    /* DEBUG_V256(pattern); */

    pattern = mul_64bit_64bit_small(pattern, _mm256_set1_epi64x(i));
    /* DEBUG_V256(pattern); */

    result =
        _mm256_add_epi64(result, _mm256_sllv_epi64(pattern, left_shift_amount));
    /* DEBUG_V256(result); */
  }
  return result;
}

static __m256i sort_nibbles_4way(__m256i array) {
  __m256i count = count_nibbles(array), position = scan_left(count),
          easy_sorted = insertion(count, position);

  // now we check the dumb case of 16 counts of the same value.
  __m256i all_same = _mm256_sub_epi64(
      _mm256_set1_epi64x(1),
      _mm256_add_epi64(_mm256_cmpeq_epi64(
                           mul_64bit_64bit_small(
                               _mm256_set1_epi64x(0x1111111111111111ULL),
                               _mm256_and_si256(array, _mm256_set1_epi64x(15))),
                           array),
                       _mm256_set1_epi64x(1)));

  /* DEBUG_V256(all_same); */
  /* DEBUG_V256(easy_sorted); */

  __m256i t = _mm256_sub_epi64(_mm256_set1_epi64x(1), all_same);
  __m256i a = mul_64bit_64bit_small(easy_sorted, t),
          b = mul_64bit_64bit_small(array, all_same);

  /* DEBUG_V256(t); */
  /* DEBUG_V256(a); */
  /* DEBUG_V256(b); */

  return _mm256_add_epi64(a, b);
}

static __m256i test_sort_nibbles_4way(__m256i input) {
  /* DEBUG_V256(input); */
  __m256i output = sort_nibbles_4way(input);
  /* DEBUG_V256(output); */
  return output;
}

static void test_mul64() {
  uint64_t m1[4] __attribute__((aligned(32))) = {
      0x0000000000000001ULL, 0x0000000000000001ULL, 0x0000000000000001ULL,
      0x0000000000000001ULL};

  uint64_t m2[4] __attribute__((aligned(32))) = {
      0x123456789abcdef0ULL, 0x123456789abcdef0ULL, 0x123456789abcdef0ULL,
      0x123456789abcdef0ULL,
  };

  __m256i t1 = _mm256_load_si256((__m256i const *)(m1));
  __m256i t2 = _mm256_load_si256((__m256i const *)(m2));
  __m256i result = mul_64bit_64bit_small(t2, t1);
}

static __m256i get_test_data(int i) {
  uint64_t m[][4] __attribute__((aligned(32))) = {
      {0x123456789abcdef0ULL, 0x0123456789ABCDEFULL, 0x0123456776543210ULL,
       0x0000000000000001ULL},
      {0x5e4e9f6e4d2f5468ULL, 0x5e4e9f6e4d2f5468ULL, 0x5e4e9f6e4d2f5468ULL,
       0x5e4e9f6e4d2f5468ULL},
      {0x3d15d3517f5f1bbbULL, 0x5e4e9f6e4d2f5468ULL, 0x0123456789ABCDEFULL,
       0x0123456776543210ULL},
      {0x1111111155555555ULL, 0x1111111144444444ULL, 0x1111111444444444ULL,
       0x1111111555555555ULL},
      {0x1111111133333333ULL, 0x1111111122222222ULL, 0x1111111222222222ULL,
       0x1111111333333333ULL},
      {0x1111111111111111ULL, 0x2222222222222222ULL, 0x3333333333333333ULL,
       0x4444444444444444ULL},
  };
  return _mm256_load_si256((__m256i const *)(m[i]));
}

static void test_count_nibbles() { count_nibbles(get_test_data(0)); }

static void test_scan_left() { scan_left(count_nibbles(get_test_data(0))); }

static void test_insertion() {
  __m256i data = get_test_data(1), count = count_nibbles(data),
          position = scan_left(count);
  insertion(count, position);
}

void nibble_sort_carlos(long *buf) {
  int i;
#pragma unroll
  for (i = 0; i < 1024; i += 4) {
    _mm256_store_si256(
        (__m256i *)(buf + i),
        sort_nibbles_4way(_mm256_load_si256((__m256i const *)(buf + i))));
  }
}

/* int main() */
/* { */
/*     test_sort_nibbles_4way(get_test_data(5)); */
/*     return 0; */
/* } */
