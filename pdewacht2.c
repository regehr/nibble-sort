#include <immintrin.h>

#include "nibble.h"

void nibble_sort_pdewacht2(unsigned long *buf) {
  __m256i *vectors = (__m256i *)buf;
  __m256i nibbles_mask = _mm256_set1_epi8(0x0F);
  __m256i last_nibble_mask = _mm256_set1_epi64x(0x0F00000000000000UL);

  for (int i = 0; i < 256; ++i) {
    __m256i input = _mm256_load_si256(&vectors[i]);
    __m256i even = _mm256_and_si256(input, nibbles_mask);
    __m256i odd = _mm256_and_si256(_mm256_srli_epi64(input, 4), nibbles_mask);

    for (int j = 0; j < 8; ++j) {
      __m256i x, y, pinky;
      x = _mm256_min_epi8(even, odd);
      y = _mm256_max_epi8(even, odd);
      pinky = _mm256_and_si256(y, last_nibble_mask);
      y = _mm256_slli_epi64(y, 8);
      even = _mm256_max_epi8(x, y);
      odd = _mm256_min_epi8(x, y);
      odd = _mm256_or_si256(_mm256_srli_epi64(odd, 8), pinky);
    }

    __m256i result = _mm256_or_si256(even, _mm256_slli_epi64(odd, 4));
    _mm256_store_si256(&vectors[i], result);
  }
}
