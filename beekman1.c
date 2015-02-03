/**
 * Copyright (C) 2015 Jethro G. Beekman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <x86intrin.h>
#include <stdint.h>

#include "nibble.h"

void nibble_sort_beekman1(uint64_t *buf) {
  // already in the right order
  //__m256i
  // shuf0={0x1716151413121110ULL,0x1f1e1d1c1b1a1918ULL,0x0706050403020100ULL,0x0f0e0d0c0b0a0908ULL};
  __m256i shuf1 = {0x1e161c141a121810ULL, 0x1f171d151b131911ULL,
                   0x0e060c040a020800ULL, 0x0f070d050b030901ULL};
  __m256i shuf2 = {0x1d1c151419181110ULL, 0x1f1e17161b1a1312ULL,
                   0x0d0c050409080100ULL, 0x0f0e07060b0a0302ULL};
  // use less instructions below
  //__m256i
  // shuf3={0x1b1a191813121110ULL,0x1f1e1d1c17161514ULL,0x0b0a090803020100ULL,0x0f0e0d0c07060504ULL};
  __m256i shuf4 = {0x101d171615141311ULL, 0x1f1e1b191a181c12ULL,
                   0x000d070605040301ULL, 0x0f0e0b090a080c02ULL};
  __m256i shuf5 = {0x171d151413111810ULL, 0x1f1e16191c1b1a12ULL,
                   0x070d050403010800ULL, 0x0f0e06090c0b0a02ULL};
  __m256i shuf6 = {0x1e17161a15141211ULL, 0x1f101d1c1b191318ULL,
                   0x0e07060a05040201ULL, 0x0f000d0c0b090308ULL};
  __m256i shuf7 = {0x171510161b131911ULL, 0x1f1d181e1c141a12ULL,
                   0x070500060b030901ULL, 0x0f0d080e0c040a02ULL};
  __m256i shuf8 = {0x1715141613121110ULL, 0x1f1e1c1b1a19181dULL,
                   0x0705040603020100ULL, 0x0f0e0c0b0a09080dULL};
  __m256i shuf9 = {0x171c1b1a19181615ULL, 0x1f1e14131211101dULL,
                   0x070c0b0a09080605ULL, 0x0f0e04030201000dULL};
  __m256i nibblemask = _mm256_set1_epi8(0x0f);
  for (uint32_t i = 0; i < (1024 / 4); i += 1) {
    __m256i r0 = _mm256_loadu_si256(((__m256i *)buf) + i), r1 = r0, r2;
    r0 &= nibblemask;
    r1 ^= r0;
    r1 = _mm256_srli_epi64(r1, 4);

#define sort_and_shuffle(n)                                                    \
  r2 = _mm256_max_epi8(r0, r1);                                                \
  r0 = _mm256_min_epi8(r0, r1);                                                \
  r1 = (__m256i)_mm256_shuffle_pd((__m256d)r0, (__m256d)r2, 0b0000);           \
  r2 = (__m256i)_mm256_shuffle_pd((__m256d)r0, (__m256d)r2, 0b1111);           \
  r1 = _mm256_shuffle_epi8(r1, shuf##n);                                       \
  r2 = _mm256_shuffle_epi8(r2, shuf##n);                                       \
  r0 = (__m256i)_mm256_shuffle_pd((__m256d)r1, (__m256d)r2, 0b0000);           \
  r1 = (__m256i)_mm256_shuffle_pd((__m256d)r1, (__m256d)r2, 0b1111)

    sort_and_shuffle(1);
    sort_and_shuffle(2);
    { // sort_and_shuffle(3);
      r2 = _mm256_max_epi8(r0, r1);
      r0 = _mm256_min_epi8(r0, r1);
      r1 = (__m256i)_mm256_unpacklo_ps((__m256)r0, (__m256)r2);
      r2 = (__m256i)_mm256_unpackhi_ps((__m256)r0, (__m256)r2);
      r0 = (__m256i)_mm256_shuffle_pd((__m256d)r1, (__m256d)r2, 0b1111);
      r1 = (__m256i)_mm256_shuffle_pd((__m256d)r1, (__m256d)r2, 0b0000);
    }
    sort_and_shuffle(4);
    sort_and_shuffle(5);
    sort_and_shuffle(6);
    sort_and_shuffle(7);
    sort_and_shuffle(8);
    sort_and_shuffle(9);

    r1 = _mm256_slli_epi64(r1, 4);
    _mm256_storeu_si256(((__m256i *)buf) + i, r1 | r0);
  }
}
