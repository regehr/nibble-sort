#include <stdint.h>

#include <tmmintrin.h>
#include <smmintrin.h>
#include <emmintrin.h>
#include <mmintrin.h>

#define SWAPS 10

static __m128i shuffle_a[SWAPS], shuffle_b[SWAPS], min[SWAPS];

struct swap {
  int a, b;
};

#define INDEX(i) ((((i)&1) * 8) + 7 - (i) / 2)

static void init1(int index, int len, const struct swap *swaps) {
  uint8_t *a = (uint8_t *)(shuffle_a + index);
  uint8_t *b = (uint8_t *)(shuffle_b + index);
  uint8_t *m = (uint8_t *)(min + index);

  for (int i = 0; i < 16; i++)
    a[i] = b[i] = i;
  for (int i = 0; i < len; i++) {
    a[INDEX(swaps[i].a)] = a[INDEX(swaps[i].b)] = INDEX(swaps[i].a);
    b[INDEX(swaps[i].a)] = b[INDEX(swaps[i].b)] = INDEX(swaps[i].b);
    m[INDEX(swaps[i].a)] = 0xff;
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

static inline void sort8(uint64_t *val) {
  __m128i v[8], mask;

  __asm__("movq        %[m64], %[m128]"
          "\n\t"
          "punpcklbw   %[m128], %[m128]"
          "\n\t"
          : [m128] "=x"(mask)
          : [m64] "r"(0x0f0f0f0f0f0f0f0fUL));

  __asm__("movdqa      (%[input]), %[v0]"
          "\n\t"
          "vpsrld      $4, %[v0], %%xmm8"
          "\n\t"
          "pand        %[mask], %[v0]"
          "\n\t"
          "pand        %[mask], %%xmm8"
          "\n\t"
          "vpunpckhbw  %%xmm8, %[v0], %[v1]"
          "\n\t"
          "vpunpcklbw  %%xmm8, %[v0], %[v0]"
          "\n\t"

          "movdqa      16(%[input]), %[v2]"
          "\n\t"
          "vpsrld      $4, %[v2], %%xmm8"
          "\n\t"
          "pand        %[mask], %[v2]"
          "\n\t"
          "pand        %[mask], %%xmm8"
          "\n\t"
          "vpunpckhbw  %%xmm8, %[v2], %[v3]"
          "\n\t"
          "vpunpcklbw  %%xmm8, %[v2], %[v2]"
          "\n\t"

          "movdqa      32(%[input]), %[v4]"
          "\n\t"
          "vpsrld      $4, %[v4], %%xmm8"
          "\n\t"
          "pand        %[mask], %[v4]"
          "\n\t"
          "pand        %[mask], %%xmm8"
          "\n\t"
          "vpunpckhbw  %%xmm8, %[v4], %[v5]"
          "\n\t"
          "vpunpcklbw  %%xmm8, %[v4], %[v4]"
          "\n\t"

          "movdqa      48(%[input]), %[v6]"
          "\n\t"
          "vpsrld      $4, %[v6], %%xmm8"
          "\n\t"
          "pand        %[mask], %[v6]"
          "\n\t"
          "pand        %[mask], %%xmm8"
          "\n\t"
          "vpunpckhbw  %%xmm8, %[v6], %[v7]"
          "\n\t"
          "vpunpcklbw  %%xmm8, %[v6], %[v6]"
          "\n\t"
          : [v0] "=x"(v[0]), [v1] "=x"(v[1]), [v2] "=x"(v[2]), [v3] "=x"(v[3]),
            [v4] "=x"(v[4]), [v5] "=x"(v[5]), [v6] "=x"(v[6]), [v7] "=x"(v[7])
          : [input] "r"(val), [mask] "x"(mask)
          : "xmm8");

  for (int i = 0; i < SWAPS; i++) {
    __asm__("vpshufb   %[shuffle_b], %[v0], %%xmm15"
            "\n\t"
            "vpshufb   %[shuffle_b], %[v1], %%xmm13"
            "\n\t"
            "vpshufb   %[shuffle_a], %[v0], %[v0]"
            "\n\t"
            "vpshufb   %[shuffle_a], %[v1], %[v1]"
            "\n\t"
            "vpcmpgtb  %%xmm15, %[v0], %%xmm14"
            "\n\t"
            "vpcmpgtb  %%xmm13, %[v1], %%xmm12"
            "\n\t"
            "vpxor     %[min], %%xmm14, %%xmm14"
            "\n\t"
            "vpxor     %[min], %%xmm12, %%xmm12"
            "\n\t"
            "vpblendvb %%xmm14, %%xmm15, %[v0], %[v0]"
            "\n\t"
            "vpblendvb %%xmm12, %%xmm13, %[v1], %[v1]"
            "\n\t"

            "vpshufb   %[shuffle_b], %[v2], %%xmm15"
            "\n\t"
            "vpshufb   %[shuffle_b], %[v3], %%xmm13"
            "\n\t"
            "vpshufb   %[shuffle_a], %[v2], %[v2]"
            "\n\t"
            "vpshufb   %[shuffle_a], %[v3], %[v3]"
            "\n\t"
            "vpcmpgtb  %%xmm15, %[v2], %%xmm14"
            "\n\t"
            "vpcmpgtb  %%xmm13, %[v3], %%xmm12"
            "\n\t"
            "vpxor     %[min], %%xmm14, %%xmm14"
            "\n\t"
            "vpxor     %[min], %%xmm12, %%xmm12"
            "\n\t"
            "vpblendvb %%xmm14, %%xmm15, %[v2], %[v2]"
            "\n\t"
            "vpblendvb %%xmm12, %%xmm13, %[v3], %[v3]"
            "\n\t"

            "vpshufb   %[shuffle_b], %[v4], %%xmm15"
            "\n\t"
            "vpshufb   %[shuffle_b], %[v5], %%xmm13"
            "\n\t"
            "vpshufb   %[shuffle_a], %[v4], %[v4]"
            "\n\t"
            "vpshufb   %[shuffle_a], %[v5], %[v5]"
            "\n\t"
            "vpcmpgtb  %%xmm15, %[v4], %%xmm14"
            "\n\t"
            "vpcmpgtb  %%xmm13, %[v5], %%xmm12"
            "\n\t"
            "vpxor     %[min], %%xmm14, %%xmm14"
            "\n\t"
            "vpxor     %[min], %%xmm12, %%xmm12"
            "\n\t"
            "vpblendvb %%xmm14, %%xmm15, %[v4], %[v4]"
            "\n\t"
            "vpblendvb %%xmm12, %%xmm13, %[v5], %[v5]"
            "\n\t"

            "vpshufb   %[shuffle_b], %[v6], %%xmm15"
            "\n\t"
            "vpshufb   %[shuffle_b], %[v7], %%xmm13"
            "\n\t"
            "vpshufb   %[shuffle_a], %[v6], %[v6]"
            "\n\t"
            "vpshufb   %[shuffle_a], %[v7], %[v7]"
            "\n\t"
            "vpcmpgtb  %%xmm15, %[v6], %%xmm14"
            "\n\t"
            "vpcmpgtb  %%xmm13, %[v7], %%xmm12"
            "\n\t"
            "vpxor     %[min], %%xmm14, %%xmm14"
            "\n\t"
            "vpxor     %[min], %%xmm12, %%xmm12"
            "\n\t"
            "vpblendvb %%xmm14, %%xmm15, %[v6], %[v6]"
            "\n\t"
            "vpblendvb %%xmm12, %%xmm13, %[v7], %[v7]"
            "\n\t"
            : [v0] "+x"(v[0]), [v1] "+x"(v[1]), [v2] "+x"(v[2]),
              [v3] "+x"(v[3]), [v4] "+x"(v[4]), [v5] "+x"(v[5]),
              [v6] "+x"(v[6]), [v7] "+x"(v[7])
            : [shuffle_a] "x"(shuffle_a[i]), [shuffle_b] "x"(shuffle_b[i]),
              [min] "x"(min[i])
            : "xmm15", "xmm14", "xmm13", "xmm12");
  }

  __asm__ __volatile__("vpunpckhqdq %[v1], %[v0], %%xmm15"
                       "\n\t"
                       "vpunpcklqdq %[v1], %[v0], %[v0]"
                       "\n\t"
                       "pslld       $4, %[v0]"
                       "\n\t"
                       "por         %%xmm15, %[v0]"
                       "\n\t"
                       "movdqa      %[v0], (%[output])"
                       "\n\t"

                       "vpunpckhqdq %[v3], %[v2], %%xmm15"
                       "\n\t"
                       "vpunpcklqdq %[v3], %[v2], %[v2]"
                       "\n\t"
                       "pslld       $4, %[v2]"
                       "\n\t"
                       "por         %%xmm15, %[v2]"
                       "\n\t"
                       "movdqa      %[v2], 16(%[output])"
                       "\n\t"

                       "vpunpckhqdq %[v5], %[v4], %%xmm15"
                       "\n\t"
                       "vpunpcklqdq %[v5], %[v4], %[v4]"
                       "\n\t"
                       "pslld       $4, %[v4]"
                       "\n\t"
                       "por         %%xmm15, %[v4]"
                       "\n\t"
                       "movdqa      %[v4], 32(%[output])"
                       "\n\t"

                       "vpunpckhqdq %[v7], %[v6], %%xmm15"
                       "\n\t"
                       "vpunpcklqdq %[v7], %[v6], %[v6]"
                       "\n\t"
                       "pslld       $4, %[v6]"
                       "\n\t"
                       "por         %%xmm15, %[v6]"
                       "\n\t"
                       "movdqa      %[v6], 48(%[output])"
                       "\n\t"
                       : [v0] "+x"(v[0]), [v1] "+x"(v[1]), [v2] "+x"(v[2]),
                         [v3] "+x"(v[3]), [v4] "+x"(v[4]), [v5] "+x"(v[5]),
                         [v6] "+x"(v[6]), [v7] "+x"(v[7])
                       : [output] "r"(val)
                       : "memory", "xmm15");
}

void nibble_sort_koorogi1(unsigned long *buf) {
  for (int i = 0; i < 1024; i += 8)
    sort8(buf + i);
}
