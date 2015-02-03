#include "nibble.h"

#define BUFFER_SIZE 1024

void nibble_sort_anon(unsigned long *buf) {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    unsigned long val = buf[i], hist = 0, newval = 0;
    int count;

    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;

    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;

    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;

    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;
    hist += (1LU << ((val & 0xf) << 2));
    val >>= 4;

    if (!(hist & (hist - 1)))
      continue;

    hist >>= 4;
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0x1111111111111111LU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0x2222222222222222LU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0x3333333333333333LU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0x4444444444444444LU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0x5555555555555555LU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0x6666666666666666LU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0x7777777777777777LU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0x8888888888888888LU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0x9999999999999999LU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0xaaaaaaaaaaaaaaaaLU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0xbbbbbbbbbbbbbbbbLU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0xccccccccccccccccLU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0xddddddddddddddddLU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0xeeeeeeeeeeeeeeeeLU & ~(-1LU >> count));
    count = (hist & 0xf) << 2;
    hist >>= 4;
    newval = (newval >> count) | (0xffffffffffffffffLU & ~(-1LU >> count));

    buf[i] = newval;
  }
}
