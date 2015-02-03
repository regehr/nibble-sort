#include "stdio.h"

typedef unsigned long w64;
#define BUFSIZE 1024

typedef struct _tuple { w64 a, b; } tuple;

static tuple unpack(w64 in) {
  w64 a = (in)&0x0F0F0F0F0F0F0F0FL;
  w64 b = (in >> 4) & 0x0F0F0F0F0F0F0F0FL;
  tuple x;
  x.a = a;
  x.b = b;
  return x;
}

static w64 pack(tuple in) { return in.a | (in.b << 4); }

static tuple sort_pair(tuple T) {
  w64 a = T.a;
  w64 b = T.b;
  w64 mask = (b - a) >> 4;
  w64 high = (mask & a) | (~mask & b);
  w64 low = (mask & b) | (~mask & a);
  w64 a2 = low;
  w64 b2 = high << 8;
  w64 mask2 = (b2 - a2) >> 4;
  w64 low2 = (mask2 & a2) | (~mask2 & b2);
  w64 high2 = (mask2 & b2) | (~mask2 & a2);
  a = low2;
  b = (high2 >> 8) | (high & 0xFF00000000000000L);
  tuple r;
  r.a = a;
  r.b = b;
  return r;
}

static void nibble_sort_nadav_w4(w64 *pIn) {
  tuple T0 = unpack(*(pIn + 0));
  tuple T1 = unpack(*(pIn + 1));
  tuple T2 = unpack(*(pIn + 2));
  tuple T3 = unpack(*(pIn + 3));

  for (int i = 0; i < 8; i++) {
    T0 = sort_pair(T0);
    T1 = sort_pair(T1);
    T2 = sort_pair(T2);
    T3 = sort_pair(T3);
  }

  *(pIn + 0) = pack(T0);
  *(pIn + 1) = pack(T1);
  *(pIn + 2) = pack(T2);
  *(pIn + 3) = pack(T3);
}

void nibble_sort_nadav(unsigned long *buf) {
  for (int i = 0; i < 1024; i += 4)
    nibble_sort_nadav_w4(&buf[i]);
}
