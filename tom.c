#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <stdint.h>

#define TEST_SIZE (1024)

/**** SSE sorting network
 * ******************************************************/

static __m128i g_mask;
static __m128i g_shuffles[6][4];
static __m128i g_shuffleout_a, g_shuffleout_b;

static const __m128i MakeShuffler(uint8_t i0, uint8_t i1, uint8_t i2,
                                  uint8_t i3, uint8_t i4, uint8_t i5,
                                  uint8_t i6, uint8_t i7, uint8_t i8,
                                  uint8_t i9, uint8_t i10, uint8_t i11,
                                  uint8_t i12, uint8_t i13, uint8_t i14,
                                  uint8_t i15) {
  return _mm_set_epi8(i15, i14, i13, i12, i11, i10, i9, i8, i7, i6, i5, i4, i3,
                      i2, i1, i0);
}

static void __attribute__((constructor)) InitConstants() {
  char a = 10, b = 11, c = 12, d = 13, e = 14, f = 15, x = 0x80;

  g_shuffles[0][0] =
      MakeShuffler(0, 2, 4, 6, 8, a, c, e, x, x, x, x, x, x, x, x);
  g_shuffles[0][1] =
      MakeShuffler(1, 3, 5, 7, 9, b, d, f, x, x, x, x, x, x, x, x);
  g_shuffles[0][2] =
      MakeShuffler(0, x, 1, x, 2, x, 3, x, 4, x, 5, x, 6, x, 7, x);
  g_shuffles[0][3] =
      MakeShuffler(x, 0, x, 1, x, 2, x, 3, x, 4, x, 5, x, 6, x, 7);

  g_shuffles[1][0] =
      MakeShuffler(0, 1, 4, 5, 8, 9, c, d, x, x, x, x, x, x, x, x);
  g_shuffles[1][1] =
      MakeShuffler(3, 2, 7, 6, b, a, f, e, x, x, x, x, x, x, x, x);
  g_shuffles[1][2] =
      MakeShuffler(0, 1, x, x, 2, 3, x, x, 4, 5, x, x, 6, 7, x, x);
  g_shuffles[1][3] =
      MakeShuffler(x, x, 1, 0, x, x, 3, 2, x, x, 5, 4, x, x, 7, 6);

  g_shuffles[2][0] =
      MakeShuffler(0, 1, 2, 3, 8, 9, a, b, x, x, x, x, x, x, x, x);
  g_shuffles[2][1] =
      MakeShuffler(7, 6, 5, 4, f, e, d, c, x, x, x, x, x, x, x, x);
  g_shuffles[2][2] =
      MakeShuffler(0, 1, 2, 3, x, x, x, x, 4, 5, 6, 7, x, x, x, x);
  g_shuffles[2][3] =
      MakeShuffler(x, x, x, x, 3, 2, 1, 0, x, x, x, x, 7, 6, 5, 4);

  g_shuffles[3][0] =
      MakeShuffler(0, 1, 4, 5, 8, 9, c, d, x, x, x, x, x, x, x, x);
  g_shuffles[3][1] =
      MakeShuffler(2, 3, 6, 7, a, b, e, f, x, x, x, x, x, x, x, x);
  g_shuffles[3][2] =
      MakeShuffler(0, 1, x, x, 2, 3, x, x, 4, 5, x, x, 6, 7, x, x);
  g_shuffles[3][3] =
      MakeShuffler(x, x, 0, 1, x, x, 2, 3, x, x, 4, 5, x, x, 6, 7);

  g_shuffles[4][0] =
      MakeShuffler(0, 1, 2, 3, 4, 5, 6, 7, x, x, x, x, x, x, x, x);
  g_shuffles[4][1] =
      MakeShuffler(f, e, d, c, b, a, 9, 8, x, x, x, x, x, x, x, x);
  g_shuffles[4][2] =
      MakeShuffler(0, 1, 2, 3, 4, 5, 6, 7, x, x, x, x, x, x, x, x);
  g_shuffles[4][3] =
      MakeShuffler(x, x, x, x, x, x, x, x, 7, 6, 5, 4, 3, 2, 1, 0);

  g_shuffles[5][0] =
      MakeShuffler(0, 1, 2, 3, 8, 9, a, b, x, x, x, x, x, x, x, x);
  g_shuffles[5][1] =
      MakeShuffler(4, 5, 6, 7, c, d, e, f, x, x, x, x, x, x, x, x);
  g_shuffles[5][2] =
      MakeShuffler(0, 1, 2, 3, x, x, x, x, 4, 5, 6, 7, x, x, x, x);
  g_shuffles[5][3] =
      MakeShuffler(x, x, x, x, 0, 1, 2, 3, x, x, x, x, 4, 5, 6, 7);

  g_shuffleout_a = MakeShuffler(0, 2, 4, 6, 8, a, c, e, x, x, x, x, x, x, x, x);
  g_shuffleout_b = MakeShuffler(1, 3, 5, 7, 9, b, d, f, x, x, x, x, x, x, x, x);

  g_mask = _mm_set_epi8(f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f);
}

static __m128i S(__m128i x, int i) {
  const __m128i a0 = _mm_shuffle_epi8(x, g_shuffles[i][0]);
  const __m128i b0 = _mm_shuffle_epi8(x, g_shuffles[i][1]);

  const __m128i a1 = _mm_min_epi8(a0, b0);
  const __m128i b1 = _mm_max_epi8(a0, b0);

  const __m128i a2 = _mm_shuffle_epi8(a1, g_shuffles[i][2]);
  const __m128i b2 = _mm_shuffle_epi8(b1, g_shuffles[i][3]);

  return _mm_or_si128(a2, b2);
}

void nibble_sort_tom(unsigned long *buf) {
  for (int i = 0; i < TEST_SIZE; ++i) {
    __m128i x = _mm_and_si128(_mm_set_epi64x(buf[i] >> 4, buf[i]), g_mask);

    x = S(x, 0);
    x = S(x, 1);
    x = S(x, 0);
    x = S(x, 2);
    x = S(x, 3);
    x = S(x, 0);
    x = S(x, 4);
    x = S(x, 5);
    x = S(x, 3);

    /* Final step is different; the output is in the right layout
     * for reassembling for the final write. */
    const __m128i a0 = _mm_shuffle_epi8(x, g_shuffles[0][0]);
    const __m128i b0 = _mm_shuffle_epi8(x, g_shuffles[0][1]);

    const __m128i a1 = _mm_min_epi8(a0, b0);
    const __m128i b1 = _mm_max_epi8(a0, b0);

    const __m128i out = _mm_or_si128(a1, _mm_slli_epi64(b1, 4));
    _mm_storel_epi64((__m128i *)&buf[i], out);
  }
}
