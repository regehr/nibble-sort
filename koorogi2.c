#include <stdint.h>
#include <immintrin.h>

#include "nibble.h"

#define SWAPS 10

static __m256i shuffle_a[SWAPS], shuffle_b[SWAPS], min[SWAPS];
static __m256i mask = {0x0f0f0f0f0f0f0f0fUL, 0x0f0f0f0f0f0f0f0fUL,
                       0x0f0f0f0f0f0f0f0fUL, 0x0f0f0f0f0f0f0f0fUL};

struct swap {
  int a, b;
};

#define INDEX(i) ((((i)&1) * 8) + 7 - (i) / 2)

static void init1(int index, int len, const struct swap *swaps) {
  uint8_t *a = (uint8_t *)(shuffle_a + index);
  uint8_t *b = (uint8_t *)(shuffle_b + index);
  uint8_t *m = (uint8_t *)(min + index);

  for (int i = 0; i < 32; i++)
    a[i] = b[i] = i;
  for (int i = 0; i < len; i++) {
    a[INDEX(swaps[i].a)] = a[INDEX(swaps[i].b)] = INDEX(swaps[i].a);
    b[INDEX(swaps[i].a)] = b[INDEX(swaps[i].b)] = INDEX(swaps[i].b);
    m[INDEX(swaps[i].a)] = 0xff;

    a[16 + INDEX(swaps[i].a)] = a[16 + INDEX(swaps[i].b)] =
        16 + INDEX(swaps[i].a);
    b[16 + INDEX(swaps[i].a)] = b[16 + INDEX(swaps[i].b)] =
        16 + INDEX(swaps[i].b);
    m[16 + INDEX(swaps[i].a)] = 0xff;
  }
}

__attribute__((constructor)) static void init_shuffle() {
  init1(0, 8, (struct swap[]){{0, 1},
                              {2, 3},
                              {4, 5},
                              {6, 7},
                              {8, 9},
                              {10, 11},
                              {12, 13},
                              {14, 15}});
  init1(1, 8, (struct swap[]){{0, 2},
                              {4, 6},
                              {8, 10},
                              {12, 14},
                              {1, 3},
                              {5, 7},
                              {9, 11},
                              {13, 15}});
  init1(2, 8, (struct swap[]){{0, 4},
                              {8, 12},
                              {1, 5},
                              {9, 13},
                              {2, 6},
                              {10, 14},
                              {3, 7},
                              {11, 15}});
  init1(3, 8, (struct swap[]){{0, 8},
                              {1, 9},
                              {2, 10},
                              {3, 11},
                              {4, 12},
                              {5, 13},
                              {6, 14},
                              {7, 15}});
  init1(4, 7, (struct swap[]){
                  {5, 10}, {6, 9}, {3, 12}, {13, 14}, {7, 11}, {1, 2}, {4, 8}});
  init1(5, 6,
        (struct swap[]){{1, 4}, {7, 13}, {2, 8}, {11, 14}, {5, 6}, {9, 10}});
  init1(6, 4, (struct swap[]){{2, 4}, {11, 13}, {3, 8}, {7, 12}});
  init1(7, 4, (struct swap[]){{6, 8}, {10, 12}, {3, 5}, {7, 9}});
  init1(8, 5, (struct swap[]){{3, 4}, {5, 6}, {7, 8}, {9, 10}, {11, 12}});
  init1(9, 2, (struct swap[]){{6, 7}, {8, 9}});
}

static inline void sort16(uint64_t *val) {
  __m256i v[8];

  __asm__("vmovdqa     (%[input]), %t[v0]"
          "\n\t"
          "vpsrld      $4, %t[v0], %%ymm8"
          "\n\t"
          "vpand       %t[mask], %t[v0], %t[v0]"
          "\n\t"
          "vpand       %t[mask], %%ymm8, %%ymm8"
          "\n\t"
          "vpunpckhbw  %%ymm8, %t[v0], %t[v1]"
          "\n\t"
          "vpunpcklbw  %%ymm8, %t[v0], %t[v0]"
          "\n\t"

          "vmovdqa     32(%[input]), %t[v2]"
          "\n\t"
          "vpsrld      $4, %t[v2], %%ymm8"
          "\n\t"
          "vpand       %t[mask], %t[v2], %t[v2]"
          "\n\t"
          "vpand       %t[mask], %%ymm8, %%ymm8"
          "\n\t"
          "vpunpckhbw  %%ymm8, %t[v2], %t[v3]"
          "\n\t"
          "vpunpcklbw  %%ymm8, %t[v2], %t[v2]"
          "\n\t"

          "vmovdqa     64(%[input]), %t[v4]"
          "\n\t"
          "vpsrld      $4, %t[v4], %%ymm8"
          "\n\t"
          "vpand       %t[mask], %t[v4], %t[v4]"
          "\n\t"
          "vpand       %t[mask], %%ymm8, %%ymm8"
          "\n\t"
          "vpunpckhbw  %%ymm8, %t[v4], %t[v5]"
          "\n\t"
          "vpunpcklbw  %%ymm8, %t[v4], %t[v4]"
          "\n\t"

          "vmovdqa     96(%[input]), %t[v6]"
          "\n\t"
          "vpsrld      $4, %t[v6], %%ymm8"
          "\n\t"
          "vpand       %t[mask], %t[v6], %t[v6]"
          "\n\t"
          "vpand       %t[mask], %%ymm8, %%ymm8"
          "\n\t"
          "vpunpckhbw  %%ymm8, %t[v6], %t[v7]"
          "\n\t"
          "vpunpcklbw  %%ymm8, %t[v6], %t[v6]"
          "\n\t"
          : [v0] "=x"(v[0]), [v1] "=x"(v[1]), [v2] "=x"(v[2]), [v3] "=x"(v[3]),
            [v4] "=x"(v[4]), [v5] "=x"(v[5]), [v6] "=x"(v[6]), [v7] "=x"(v[7])
          : [input] "r"(val), [mask] "x"(mask)
          : "xmm8");

  for (int i = 0; i < SWAPS; i++) {
    __asm__("vpshufb   %t[shuffle_b], %t[v0], %%ymm15"
            "\n\t"
            "vpshufb   %t[shuffle_b], %t[v1], %%ymm13"
            "\n\t"
            "vpshufb   %t[shuffle_a], %t[v0], %t[v0]"
            "\n\t"
            "vpshufb   %t[shuffle_a], %t[v1], %t[v1]"
            "\n\t"
            "vpcmpgtb  %%ymm15, %t[v0], %%ymm14"
            "\n\t"
            "vpcmpgtb  %%ymm13, %t[v1], %%ymm12"
            "\n\t"
            "vpxor     %t[min], %%ymm14, %%ymm14"
            "\n\t"
            "vpxor     %t[min], %%ymm12, %%ymm12"
            "\n\t"
            "vpblendvb %%ymm14, %%ymm15, %t[v0], %t[v0]"
            "\n\t"
            "vpblendvb %%ymm12, %%ymm13, %t[v1], %t[v1]"
            "\n\t"

            "vpshufb   %t[shuffle_b], %t[v2], %%ymm15"
            "\n\t"
            "vpshufb   %t[shuffle_b], %t[v3], %%ymm13"
            "\n\t"
            "vpshufb   %t[shuffle_a], %t[v2], %t[v2]"
            "\n\t"
            "vpshufb   %t[shuffle_a], %t[v3], %t[v3]"
            "\n\t"
            "vpcmpgtb  %%ymm15, %t[v2], %%ymm14"
            "\n\t"
            "vpcmpgtb  %%ymm13, %t[v3], %%ymm12"
            "\n\t"
            "vpxor     %t[min], %%ymm14, %%ymm14"
            "\n\t"
            "vpxor     %t[min], %%ymm12, %%ymm12"
            "\n\t"
            "vpblendvb %%ymm14, %%ymm15, %t[v2], %t[v2]"
            "\n\t"
            "vpblendvb %%ymm12, %%ymm13, %t[v3], %t[v3]"
            "\n\t"

            "vpshufb   %t[shuffle_b], %t[v4], %%ymm15"
            "\n\t"
            "vpshufb   %t[shuffle_b], %t[v5], %%ymm13"
            "\n\t"
            "vpshufb   %t[shuffle_a], %t[v4], %t[v4]"
            "\n\t"
            "vpshufb   %t[shuffle_a], %t[v5], %t[v5]"
            "\n\t"
            "vpcmpgtb  %%ymm15, %t[v4], %%ymm14"
            "\n\t"
            "vpcmpgtb  %%ymm13, %t[v5], %%ymm12"
            "\n\t"
            "vpxor     %t[min], %%ymm14, %%ymm14"
            "\n\t"
            "vpxor     %t[min], %%ymm12, %%ymm12"
            "\n\t"
            "vpblendvb %%ymm14, %%ymm15, %t[v4], %t[v4]"
            "\n\t"
            "vpblendvb %%ymm12, %%ymm13, %t[v5], %t[v5]"
            "\n\t"

            "vpshufb   %t[shuffle_b], %t[v6], %%ymm15"
            "\n\t"
            "vpshufb   %t[shuffle_b], %t[v7], %%ymm13"
            "\n\t"
            "vpshufb   %t[shuffle_a], %t[v6], %t[v6]"
            "\n\t"
            "vpshufb   %t[shuffle_a], %t[v7], %t[v7]"
            "\n\t"
            "vpcmpgtb  %%ymm15, %t[v6], %%ymm14"
            "\n\t"
            "vpcmpgtb  %%ymm13, %t[v7], %%ymm12"
            "\n\t"
            "vpxor     %t[min], %%ymm14, %%ymm14"
            "\n\t"
            "vpxor     %t[min], %%ymm12, %%ymm12"
            "\n\t"
            "vpblendvb %%ymm14, %%ymm15, %t[v6], %t[v6]"
            "\n\t"
            "vpblendvb %%ymm12, %%ymm13, %t[v7], %t[v7]"
            "\n\t"
            : [v0] "+x"(v[0]), [v1] "+x"(v[1]), [v2] "+x"(v[2]),
              [v3] "+x"(v[3]), [v4] "+x"(v[4]), [v5] "+x"(v[5]),
              [v6] "+x"(v[6]), [v7] "+x"(v[7])
            : [shuffle_a] "x"(shuffle_a[i]), [shuffle_b] "x"(shuffle_b[i]),
              [min] "x"(min[i])
            : "xmm15", "xmm14", "xmm13", "xmm12");
  }

  __asm__ __volatile__("vpunpckhqdq %t[v1], %t[v0], %%ymm15"
                       "\n\t"
                       "vpunpcklqdq %t[v1], %t[v0], %t[v0]"
                       "\n\t"
                       "vpslld      $4, %t[v0], %t[v0]"
                       "\n\t"
                       "vpor        %%ymm15, %t[v0], %t[v0]"
                       "\n\t"
                       "vmovdqa     %t[v0], (%[output])"
                       "\n\t"

                       "vpunpckhqdq %t[v3], %t[v2], %%ymm15"
                       "\n\t"
                       "vpunpcklqdq %t[v3], %t[v2], %t[v2]"
                       "\n\t"
                       "vpslld      $4, %t[v2], %t[v2]"
                       "\n\t"
                       "vpor        %%ymm15, %t[v2], %t[v2]"
                       "\n\t"
                       "vmovdqa     %t[v2], 32(%[output])"
                       "\n\t"

                       "vpunpckhqdq %t[v5], %t[v4], %%ymm15"
                       "\n\t"
                       "vpunpcklqdq %t[v5], %t[v4], %t[v4]"
                       "\n\t"
                       "vpslld      $4, %t[v4], %t[v4]"
                       "\n\t"
                       "vpor        %%ymm15, %t[v4], %t[v4]"
                       "\n\t"
                       "vmovdqa     %t[v4], 64(%[output])"
                       "\n\t"

                       "vpunpckhqdq %t[v7], %t[v6], %%ymm15"
                       "\n\t"
                       "vpunpcklqdq %t[v7], %t[v6], %t[v6]"
                       "\n\t"
                       "vpslld      $4, %t[v6], %t[v6]"
                       "\n\t"
                       "vpor        %%ymm15, %t[v6], %t[v6]"
                       "\n\t"
                       "vmovdqa     %t[v6], 96(%[output])"
                       "\n\t"
                       : [v0] "+x"(v[0]), [v1] "+x"(v[1]), [v2] "+x"(v[2]),
                         [v3] "+x"(v[3]), [v4] "+x"(v[4]), [v5] "+x"(v[5]),
                         [v6] "+x"(v[6]), [v7] "+x"(v[7])
                       : [output] "r"(val)
                       : "memory", "xmm15");
}

void nibble_sort_koorogi2(unsigned long *buf) {
  for (int i = 0; i < 1024; i += 16)
    sort16(buf + i);
}
