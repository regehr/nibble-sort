#include "nibble.h"

#include <immintrin.h>
#pragma GCC target("avx2")

typedef unsigned char u8v __attribute__((vector_size(32)));
typedef unsigned short u16v __attribute__((vector_size(32)));
typedef unsigned int u32v __attribute__((vector_size(32)));
typedef unsigned long u64v __attribute__((vector_size(32)));

typedef union {
  __m256i m;
  u8v u8;
  u16v u16;
  u32v u32;
  u64v u64;
} uv;

static inline uv x32l(uv u, uv v) {
  v.u64 <<= 32;
  return (uv)_mm256_blend_epi32(u.m, v.m, 0xAA);
}

static inline uv x32h(uv u, uv v) {
  u.u64 >>= 32;
  return (uv)_mm256_blend_epi32(u.m, v.m, 0xAA);
}

static inline uv x16l(uv u, uv v) {
  v.u32 <<= 16;
  return (uv)_mm256_blend_epi16(u.m, v.m, 0xAA);
}

static inline uv x16h(uv u, uv v) {
  u.u32 >>= 16;
  return (uv)_mm256_blend_epi16(u.m, v.m, 0xAA);
}

static inline uv x8l(uv u, uv v, uv m) {
  v.u16 <<= 8;
  u.m = _mm256_and_si256(m.m, u.m);
  return (uv)(v.u16 | u.u16);
}

static inline uv x8h(uv u, uv v, uv m) {
  u.u16 >>= 8;
  v.m = _mm256_andnot_si256(m.m, v.m);
  return (uv)(u.u16 | v.u16);
}

static inline uv fold(uv u, uv v, uv m) {
  u.u16 >>= 4;
  u.m = _mm256_and_si256(m.m, u.m);
  v.m = _mm256_andnot_si256(m.m, v.m);
  return (uv)(u.u16 | v.u16);
}

static inline uv min(uv u, uv v) { return (uv)_mm256_min_epu8(u.m, v.m); }

static inline uv max(uv u, uv v) { return (uv)_mm256_max_epu8(u.m, v.m); }

void nibble_sort_alexander4(unsigned long *buf) {
  uv *u = (uv *)buf;
  uv m = {0};
  m.u16 |= 0xFF;
  uv mf = {0};
  mf.u8 |= 0xF;
  for (int i = 0; i < 1024 / 4; i += 8) {
    uv u0 = u[i + 0];
    uv u1 = u[i + 1];
    uv u2 = u[i + 2];
    uv u3 = u[i + 3];
    uv u4 = u[i + 4];
    uv u5 = u[i + 5];
    uv u6 = u[i + 6];
    uv u7 = u[i + 7];
    uv tt;

    tt = x32l(u0, u4);
    u4 = x32h(u0, u4);
    u0 = tt;
    tt = x32l(u1, u5);
    u5 = x32h(u1, u5);
    u1 = tt;
    tt = x32l(u2, u6);
    u6 = x32h(u2, u6);
    u2 = tt;
    tt = x32l(u3, u7);
    u7 = x32h(u3, u7);
    u3 = tt;

    tt = x16l(u0, u2);
    u2 = x16h(u0, u2);
    u0 = tt;
    tt = x16l(u1, u3);
    u3 = x16h(u1, u3);
    u1 = tt;
    tt = x16l(u4, u6);
    u6 = x16h(u4, u6);
    u4 = tt;
    tt = x16l(u5, u7);
    u7 = x16h(u5, u7);
    u5 = tt;

    tt = x8l(u0, u1, m);
    u1 = x8h(u0, u1, m);
    u0 = tt;
    tt = x8l(u2, u3, m);
    u3 = x8h(u2, u3, m);
    u2 = tt;
    tt = x8l(u4, u5, m);
    u5 = x8h(u4, u5, m);
    u4 = tt;
    tt = x8l(u6, u7, m);
    u7 = x8h(u6, u7, m);
    u6 = tt;

    uv u8 = (uv)(u0.u16 << 4);
    uv u9 = (uv)(u1.u16 << 4);
    uv u10 = (uv)(u2.u16 << 4);
    uv u11 = (uv)(u3.u16 << 4);
    uv u12 = (uv)(u4.u16 << 4);
    uv u13 = (uv)(u5.u16 << 4);
    uv u14 = (uv)(u6.u16 << 4);
    uv u15 = (uv)(u7.u16 << 4);

#define SWAP(a, b)                                                             \
  tt = max(u##a, u##b);                                                        \
  u##a = min(u##a, u##b);                                                      \
  u##b = tt
    SWAP(0, 1);
    SWAP(2, 3);
    SWAP(4, 5);
    SWAP(6, 7);
    SWAP(8, 9);
    SWAP(10, 11);
    SWAP(12, 13);
    SWAP(14, 15);
    SWAP(0, 2);
    SWAP(4, 6);
    SWAP(8, 10);
    SWAP(12, 14);
    SWAP(1, 3);
    SWAP(5, 7);
    SWAP(9, 11);
    SWAP(13, 15);
    SWAP(0, 4);
    SWAP(8, 12);
    SWAP(1, 5);
    SWAP(9, 13);
    SWAP(2, 6);
    SWAP(10, 14);
    SWAP(3, 7);
    SWAP(11, 15);
    SWAP(0, 8);
    SWAP(1, 9);
    SWAP(2, 10);
    SWAP(3, 11);
    SWAP(4, 12);
    SWAP(5, 13);
    SWAP(6, 14);
    SWAP(7, 15);
    SWAP(5, 10);
    SWAP(6, 9);
    SWAP(3, 12);
    SWAP(13, 14);
    SWAP(7, 11);
    SWAP(1, 2);
    SWAP(4, 8);
    SWAP(1, 4);
    SWAP(7, 13);
    SWAP(2, 8);
    SWAP(11, 14);
    SWAP(2, 4);
    SWAP(5, 6);
    SWAP(9, 10);
    SWAP(11, 13);
    SWAP(3, 8);
    SWAP(7, 12);
    SWAP(6, 8);
    SWAP(10, 12);
    SWAP(3, 5);
    SWAP(7, 9);
    SWAP(3, 4);
    SWAP(5, 6);
    SWAP(7, 8);
    SWAP(9, 10);
    SWAP(11, 12);
    SWAP(6, 7);
    SWAP(8, 9);
#undef SWAP

    u0 = fold(u0, u1, mf);
    u1 = fold(u2, u3, mf);
    u2 = fold(u4, u5, mf);
    u3 = fold(u6, u7, mf);
    u4 = fold(u8, u9, mf);
    u5 = fold(u10, u11, mf);
    u6 = fold(u12, u13, mf);
    u7 = fold(u14, u15, mf);

    tt = x32l(u0, u4);
    u4 = x32h(u0, u4);
    u0 = tt;
    tt = x32l(u1, u5);
    u5 = x32h(u1, u5);
    u1 = tt;
    tt = x32l(u2, u6);
    u6 = x32h(u2, u6);
    u2 = tt;
    tt = x32l(u3, u7);
    u7 = x32h(u3, u7);
    u3 = tt;

    tt = x16l(u0, u2);
    u2 = x16h(u0, u2);
    u0 = tt;
    tt = x16l(u1, u3);
    u3 = x16h(u1, u3);
    u1 = tt;
    tt = x16l(u4, u6);
    u6 = x16h(u4, u6);
    u4 = tt;
    tt = x16l(u5, u7);
    u7 = x16h(u5, u7);
    u5 = tt;

    tt = x8l(u0, u1, m);
    u1 = x8h(u0, u1, m);
    u0 = tt;
    tt = x8l(u2, u3, m);
    u3 = x8h(u2, u3, m);
    u2 = tt;
    tt = x8l(u4, u5, m);
    u5 = x8h(u4, u5, m);
    u4 = tt;
    tt = x8l(u6, u7, m);
    u7 = x8h(u6, u7, m);
    u6 = tt;

    u[i + 0] = u0;
    u[i + 1] = u1;
    u[i + 2] = u2;
    u[i + 3] = u3;
    u[i + 4] = u4;
    u[i + 5] = u5;
    u[i + 6] = u6;
    u[i + 7] = u7;
  }
}
