#include <stdlib.h>

#include "nibble.h"

static unsigned long *t;

static __attribute__((constructor)) void precomp() {
  if (posix_memalign((void **)&t, 1 << 22, 1 << 15))
    abort();
  unsigned long *tt = t;
  for (unsigned long i = 1; i; i <<= 4)
    for (unsigned long j = 1; j; j <<= 4)
      for (unsigned long k = 1; k; k <<= 4)
        *tt++ = i + j + k;
}

void __attribute__((target("arch=haswell")))
nibble_sort_alexander1(unsigned long *buf) {
  typedef unsigned long u64;
  typedef unsigned long u64v __attribute__((vector_size(32)));
  for (int i = 0; i < 1024; i += 4) {
    u64v aa, vv = *(u64v *)(buf + i);
    u64 s0, s1, s2, s3;
    aa = (u64)t + ((vv & 0xFFF) << 3);
    s0 = *(u64 *)aa[0];
    s1 = *(u64 *)aa[1];
    s2 = *(u64 *)aa[2];
    s3 = *(u64 *)aa[3];
    aa = (u64)t + ((vv >> 9) & 0x7FF8);
    s0 += *(u64 *)aa[0];
    s1 += *(u64 *)aa[1];
    s2 += *(u64 *)aa[2];
    s3 += *(u64 *)aa[3];
    aa = (u64)t + ((vv >> 21) & 0x7FF8);
    s0 += *(u64 *)aa[0];
    s1 += *(u64 *)aa[1];
    s2 += *(u64 *)aa[2];
    s3 += *(u64 *)aa[3];
    aa = (u64)t + ((vv >> 33) & 0x7FF8);
    s0 += *(u64 *)aa[0];
    s1 += *(u64 *)aa[1];
    s2 += *(u64 *)aa[2];
    s3 += *(u64 *)aa[3];
    aa = (u64)t + ((vv >> 45) & 0x7FF8);
    s0 += *(u64 *)aa[0];
    s1 += *(u64 *)aa[1];
    s2 += *(u64 *)aa[2];
    s3 += *(u64 *)aa[3];
    u64v ss = {s0, s1, s2, s3};
    u64v uu = 1 << ((vv >> 60) << 2);
    ss += uu;
    uu = ss == (uu << 4);

    unsigned long m = 0x1111111111111111U;
    u64v ww, nn = (ss & 0xF) << 2, mm = {m, m, m, m};
    mm <<= nn;
    nn = (ss >> 2) & 0x3C;
    ww = mm;
    mm <<= nn;
    nn = (ss >> 6) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 10) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 14) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 18) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 22) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 26) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 30) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 34) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 38) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 42) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 46) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 50) & 0x3C;
    ww += mm;
    mm <<= nn;
    nn = (ss >> 54) & 0x3C;
    ww += mm;
    mm <<= nn;
    ww += mm;

    *(u64v *)(buf + i) = vv & uu | ww & ~uu;
  }
}
