#include "nibble.h"

#include <stdint.h>
#include <stdlib.h>

typedef uint8_t v32u8 __attribute__((vector_size(32)));
typedef uint16_t v16u16 __attribute__((vector_size(32)));
typedef uint32_t v8u32 __attribute__((vector_size(32)));
typedef uint64_t v4u64 __attribute__((vector_size(32)));
typedef int8_t v32s8 __attribute__((vector_size(32)));

#define DEFAULTNUM 1024

typedef struct sixteenvec { v32u8 vv[16]; } vv16;

static vv16 fetchvec(v32u8 const *src) {
  vv16 ret;
  for (size_t i = 0; i < 8; i++) {
    ret.vv[2 * i] = src[i];
  }
  return ret;
}

static void storevec(v32u8 *dest, vv16 src) {
  for (size_t i = 0; i < 8; i++) {
    dest[i] = src.vv[2 * i];
  }
  return;
}

static void expand(vv16 *buf) {
  for (size_t i = 0; i < 8; i++) {
    v32u8 temp0 = buf->vv[2 * i + 0];
    v32u8 temp1 = (v32u8)((v8u32)temp0 >> 4);
    buf->vv[2 * i + 0] = temp0 & 0xF;
    buf->vv[2 * i + 1] = temp1 & 0xF;
  }
  return;
}

static void compact(vv16 *buf) {
  for (size_t i = 0; i < 8; i++) {
    v32u8 temp0 = buf->vv[2 * i + 1];
    v32u8 temp1 = (v32u8)((v8u32)temp0 << 4);
    temp0 = buf->vv[2 * i + 0];
    buf->vv[2 * i + 0] = temp0 | temp1;
  }
  return;
}

static void t1bw(vv16 *ptr) {
  v32u8 const mask = {0,  32, 1,  33, 2,  34, 3,  35, 4,  36, 5,
                      37, 6,  38, 7,  39, 16, 48, 17, 49, 18, 50,
                      19, 51, 20, 52, 21, 53, 22, 54, 23, 55};
  for (size_t i = 0; i < 16; i += 4) {
    v32u8 temp0 = __builtin_shuffle(ptr->vv[i + 0], ptr->vv[i + 2], mask + 0);
    ptr->vv[i + 2] =
        __builtin_shuffle(ptr->vv[i + 0], ptr->vv[i + 2], mask + 8);
    ptr->vv[i + 0] = temp0;
  }
  return;
}

static void t2wd(vv16 *ptr) {
  v16u16 const mask = {0, 16, 1, 17, 2,  18, 3,  19,
                       8, 24, 9, 25, 10, 26, 11, 27};
  for (size_t i = 0; i < 16; i += 8)
    for (size_t j = 0; j < 4; j += 2) {
      v32u8 temp0 = (v32u8)__builtin_shuffle(
          (v16u16)(ptr->vv[i + j + 0]), (v16u16)(ptr->vv[i + j + 4]), mask + 0);
      ptr->vv[i + j + 4] = (v32u8)__builtin_shuffle(
          (v16u16)(ptr->vv[i + j + 0]), (v16u16)(ptr->vv[i + j + 4]), mask + 4);
      ptr->vv[i + j + 0] = temp0;
    }
  return;
}

static void t3dq(vv16 *ptr) {
  v8u32 const mask = {0, 8, 1, 9, 4, 12, 5, 13};
  for (size_t i = 0; i < 8; i += 2) {
    v32u8 temp0 = (v32u8)__builtin_shuffle((v8u32)ptr->vv[i + 0],
                                           (v8u32)ptr->vv[i + 8], mask + 0);
    ptr->vv[i + 8] = (v32u8)__builtin_shuffle((v8u32)ptr->vv[i + 0],
                                              (v8u32)ptr->vv[i + 8], mask + 2);
    ptr->vv[i + 0] = temp0;
  }
  return;
}

static void t4qo(vv16 *ptr) {
  v4u64 const mask = {0, 4, 2, 6};
  for (size_t i = 0; i < 16; i += 4) {
    v32u8 temp0 = (v32u8)__builtin_shuffle(
        (v4u64)ptr->vv[i + 0], (v4u64)ptr->vv[i + 2], (v4u64)mask + 0);
    ptr->vv[i + 2] = (v32u8)__builtin_shuffle((v4u64)ptr->vv[i + 0],
                                              (v4u64)ptr->vv[i + 2], mask + 1);
    ptr->vv[i + 0] = temp0;
  }
  return;
}

static void treo(vv16 *ptr) {
  v32u8 temp0 = ptr->vv[4];
  ptr->vv[4] = ptr->vv[8];
  ptr->vv[8] = temp0;
  v32u8 temp1 = ptr->vv[6];
  ptr->vv[6] = ptr->vv[10];
  ptr->vv[10] = temp1;
  return;
}

static void transpose(vv16 *ptr) {
  t1bw(ptr);
  t2wd(ptr);
  t3dq(ptr);
  t4qo(ptr);
  treo(ptr);
  return;
}

static void netsort(vv16 *ptr) {
#define CONDSWAP(x, y)                                                         \
  {                                                                            \
    v32u8 c = (v32u8)((v32s8)x >= (v32s8)y);                                   \
    v32u8 t = (x ^ y) & c;                                                     \
    y ^= t;                                                                    \
    x ^= t;                                                                    \
  }
  CONDSWAP(ptr->vv[0], ptr->vv[1]);
  CONDSWAP(ptr->vv[2], ptr->vv[3]);
  CONDSWAP(ptr->vv[4], ptr->vv[5]);
  CONDSWAP(ptr->vv[6], ptr->vv[7]);
  CONDSWAP(ptr->vv[8], ptr->vv[9]);
  CONDSWAP(ptr->vv[10], ptr->vv[11]);
  CONDSWAP(ptr->vv[12], ptr->vv[13]);
  CONDSWAP(ptr->vv[14], ptr->vv[15]);
  CONDSWAP(ptr->vv[0], ptr->vv[2]);
  CONDSWAP(ptr->vv[4], ptr->vv[6]);
  CONDSWAP(ptr->vv[8], ptr->vv[10]);
  CONDSWAP(ptr->vv[12], ptr->vv[14]);
  CONDSWAP(ptr->vv[1], ptr->vv[3]);
  CONDSWAP(ptr->vv[5], ptr->vv[7]);
  CONDSWAP(ptr->vv[9], ptr->vv[11]);
  CONDSWAP(ptr->vv[13], ptr->vv[15]);
  CONDSWAP(ptr->vv[0], ptr->vv[4]);
  CONDSWAP(ptr->vv[8], ptr->vv[12]);
  CONDSWAP(ptr->vv[1], ptr->vv[5]);
  CONDSWAP(ptr->vv[9], ptr->vv[13]);
  CONDSWAP(ptr->vv[2], ptr->vv[6]);
  CONDSWAP(ptr->vv[10], ptr->vv[14]);
  CONDSWAP(ptr->vv[3], ptr->vv[7]);
  CONDSWAP(ptr->vv[11], ptr->vv[15]);
  CONDSWAP(ptr->vv[0], ptr->vv[8]);
  CONDSWAP(ptr->vv[1], ptr->vv[9]);
  CONDSWAP(ptr->vv[2], ptr->vv[10]);
  CONDSWAP(ptr->vv[3], ptr->vv[11]);
  CONDSWAP(ptr->vv[4], ptr->vv[12]);
  CONDSWAP(ptr->vv[5], ptr->vv[13]);
  CONDSWAP(ptr->vv[6], ptr->vv[14]);
  CONDSWAP(ptr->vv[7], ptr->vv[15]);
  CONDSWAP(ptr->vv[5], ptr->vv[10]);
  CONDSWAP(ptr->vv[6], ptr->vv[9]);
  CONDSWAP(ptr->vv[3], ptr->vv[12]);
  CONDSWAP(ptr->vv[13], ptr->vv[14]);
  CONDSWAP(ptr->vv[7], ptr->vv[11]);
  CONDSWAP(ptr->vv[1], ptr->vv[2]);
  CONDSWAP(ptr->vv[4], ptr->vv[8]);
  CONDSWAP(ptr->vv[1], ptr->vv[4]);
  CONDSWAP(ptr->vv[7], ptr->vv[13]);
  CONDSWAP(ptr->vv[2], ptr->vv[8]);
  CONDSWAP(ptr->vv[11], ptr->vv[14]);
  CONDSWAP(ptr->vv[2], ptr->vv[4]);
  CONDSWAP(ptr->vv[5], ptr->vv[6]);
  CONDSWAP(ptr->vv[9], ptr->vv[10]);
  CONDSWAP(ptr->vv[11], ptr->vv[13]);
  CONDSWAP(ptr->vv[3], ptr->vv[8]);
  CONDSWAP(ptr->vv[7], ptr->vv[12]);
  CONDSWAP(ptr->vv[6], ptr->vv[8]);
  CONDSWAP(ptr->vv[10], ptr->vv[12]);
  CONDSWAP(ptr->vv[3], ptr->vv[5]);
  CONDSWAP(ptr->vv[7], ptr->vv[9]);
  CONDSWAP(ptr->vv[3], ptr->vv[4]);
  CONDSWAP(ptr->vv[5], ptr->vv[6]);
  CONDSWAP(ptr->vv[7], ptr->vv[8]);
  CONDSWAP(ptr->vv[9], ptr->vv[10]);
  CONDSWAP(ptr->vv[11], ptr->vv[12]);
  CONDSWAP(ptr->vv[6], ptr->vv[7]);
  CONDSWAP(ptr->vv[8], ptr->vv[9]);
#undef CONDSWAP
  return;
}

void nibble_sort_uwe(uint64_t *in) {
  uint64_t *vec = __builtin_assume_aligned(in, 32);
  for (size_t i = 0; i < DEFAULTNUM;
       i += ((sizeof(vv16) / 2) / sizeof(uint64_t))) {
    vv16 v16 = fetchvec((v32u8 *)&vec[i]);
    transpose(&v16);
    expand(&v16);
    netsort(&v16);
    compact(&v16);
    transpose(&v16);
    storevec((v32u8 *)&vec[i], v16);
  }
  return;
}
