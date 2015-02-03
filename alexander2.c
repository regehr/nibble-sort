#include "nibble.h"

/* argh cannot include stdlib when compiling in non-SIMD since it contains atof
 * and the calling convention puts its return value in a SIMD register
 */
extern int posix_memalign(void **__memptr, size_t __alignment,
                          size_t __size) __THROW __nonnull((1)) __wur;
extern void abort(void) __THROW __attribute__((__noreturn__));

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

void nibble_sort_alexander2(unsigned long *buf) {
  unsigned long v, nextv, s, nexts;

  nextv = buf[0];
  nexts = t[(nextv >> 0) & 0xFFF];
  nexts += t[(nextv >> 12) & 0xFFF];
  nexts += t[(nextv >> 24) & 0xFFF];
  nexts += t[(nextv >> 36) & 0xFFF];
  nexts += t[(nextv >> 48) & 0xFFF];
  nextv = 1L << 4 * (nextv >> 60);
  nexts += nextv;

  for (int i = 0; i < 1023; i++) {
    s = nexts;
    v = nextv;
    nextv = buf[i + 1];
    nexts = t[(nextv >> 0) & 0xFFF];
    nexts += t[(nextv >> 12) & 0xFFF];
    nexts += t[(nextv >> 24) & 0xFFF];
    nexts += t[(nextv >> 36) & 0xFFF];
    nexts += t[(nextv >> 48) & 0xFFF];
    nextv = 1L << 4 * (nextv >> 60);
    nexts += nextv;
    if (s == 16 * v)
      continue;
    v = 0;
    unsigned long m = 0x1111111111111111U;
    int n = (s >> 0) & 0xF;
    n *= 4;
    m <<= n;
    n = (s >> 2) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 6) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 10) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 14) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 18) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 22) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 26) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 30) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 34) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 38) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 42) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 46) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 50) & 0x3C;
    v += m;
    m <<= n;
    n = (s >> 54) & 0x3C;
    v += m;
    m <<= n;
    v += m;
    buf[i] = v;
  }
  s = nexts;
  v = nextv;
  if (s == 16 * v)
    return;
  v = 0;
  unsigned long m = 0x1111111111111111U;
  int n = (s >> 0) & 0xF;
  n *= 4;
  m <<= n;
  n = (s >> 2) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 6) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 10) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 14) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 18) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 22) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 26) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 30) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 34) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 38) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 42) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 46) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 50) & 0x3C;
  v += m;
  m <<= n;
  n = (s >> 54) & 0x3C;
  v += m;
  m <<= n;
  v += m;
  buf[1023] = v;
}
