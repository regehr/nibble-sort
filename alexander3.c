#include <stdlib.h>

#include "nibble.h"

void __attribute__((target("arch=haswell")))
nibble_sort_alexander3(unsigned long *buf) {
  typedef unsigned long u64;
  typedef unsigned long u64v __attribute__((vector_size(32)));
  for (int i = 0; i < 1024; i += 4) {
    u64v ss, uu, vv = *(u64v *)(buf + i);
    ss = 1 << ((vv & 0xF) << 2);
    ss += 1 << ((vv >> 2) & 0x3C);
    ss += 1 << ((vv >> 6) & 0x3C);
    ss += 1 << ((vv >> 10) & 0x3C);
    ss += 1 << ((vv >> 14) & 0x3C);
    ss += 1 << ((vv >> 18) & 0x3C);
    ss += 1 << ((vv >> 22) & 0x3C);
    ss += 1 << ((vv >> 26) & 0x3C);
    ss += 1 << ((vv >> 30) & 0x3C);
    ss += 1 << ((vv >> 34) & 0x3C);
    ss += 1 << ((vv >> 38) & 0x3C);
    ss += 1 << ((vv >> 42) & 0x3C);
    ss += 1 << ((vv >> 46) & 0x3C);
    ss += 1 << ((vv >> 50) & 0x3C);
    ss += 1 << ((vv >> 54) & 0x3C);
    uu = 1 << ((vv >> 58) & 0x3C);
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
