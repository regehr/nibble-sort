#include "nibble.h"

void nibble_sort_mats(unsigned long *buf) {
  for (size_t i = 0; i < 1024; i += 2) {
    int binsA[16] = {0};
    unsigned long valA = buf[i + 0];
    int binsB[16] = {0};
    unsigned long valB = buf[i + 1];
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;

    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;

    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;

    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    valA = valA >> 4;
    binsB[valB & 0xf]++;
    valB = valB >> 4;
    binsA[valA & 0xf]++;
    binsB[valB & 0xf]++;

    unsigned long resultA = 0;
    unsigned long resultB = 0;
    unsigned long maskA = ~(unsigned long)0;
    unsigned long maskB = ~(unsigned long)0;
    resultA = (resultA & ~maskA) | (maskA & 0xfffffffffffffffful);
    maskA = (binsA[0xf] > 0xf) ? 0 : maskA >> 4 * binsA[0xf];
    resultB = (resultB & ~maskB) | (maskB & 0xfffffffffffffffful);
    maskB = (binsB[0xf] > 0xf) ? 0 : maskB >> 4 * binsB[0xf];
    resultA = (resultA & ~maskA) | (maskA & 0xeeeeeeeeeeeeeeeeul);
    maskA = (binsA[0xe] > 0xf) ? 0 : maskA >> 4 * binsA[0xe];
    resultB = (resultB & ~maskB) | (maskB & 0xeeeeeeeeeeeeeeeeul);
    maskB = (binsB[0xe] > 0xf) ? 0 : maskB >> 4 * binsB[0xe];
    resultA = (resultA & ~maskA) | (maskA & 0xddddddddddddddddul);
    maskA = (binsA[0xd] > 0xf) ? 0 : maskA >> 4 * binsA[0xd];
    resultB = (resultB & ~maskB) | (maskB & 0xddddddddddddddddul);
    maskB = (binsB[0xd] > 0xf) ? 0 : maskB >> 4 * binsB[0xd];
    resultA = (resultA & ~maskA) | (maskA & 0xccccccccccccccccul);
    maskA = (binsA[0xc] > 0xf) ? 0 : maskA >> 4 * binsA[0xc];
    resultB = (resultB & ~maskB) | (maskB & 0xccccccccccccccccul);
    maskB = (binsB[0xc] > 0xf) ? 0 : maskB >> 4 * binsB[0xc];
    resultA = (resultA & ~maskA) | (maskA & 0xbbbbbbbbbbbbbbbbul);
    maskA = (binsA[0xb] > 0xf) ? 0 : maskA >> 4 * binsA[0xb];
    resultB = (resultB & ~maskB) | (maskB & 0xbbbbbbbbbbbbbbbbul);
    maskB = (binsB[0xb] > 0xf) ? 0 : maskB >> 4 * binsB[0xb];
    resultA = (resultA & ~maskA) | (maskA & 0xaaaaaaaaaaaaaaaaul);
    maskA = (binsA[0xa] > 0xf) ? 0 : maskA >> 4 * binsA[0xa];
    resultB = (resultB & ~maskB) | (maskB & 0xaaaaaaaaaaaaaaaaul);
    maskB = (binsB[0xa] > 0xf) ? 0 : maskB >> 4 * binsB[0xa];
    resultA = (resultA & ~maskA) | (maskA & 0x9999999999999999ul);
    maskA = (binsA[0x9] > 0xf) ? 0 : maskA >> 4 * binsA[0x9];
    resultB = (resultB & ~maskB) | (maskB & 0x9999999999999999ul);
    maskB = (binsB[0x9] > 0xf) ? 0 : maskB >> 4 * binsB[0x9];
    resultA = (resultA & ~maskA) | (maskA & 0x8888888888888888ul);
    maskA = (binsA[0x8] > 0xf) ? 0 : maskA >> 4 * binsA[0x8];
    resultB = (resultB & ~maskB) | (maskB & 0x8888888888888888ul);
    maskB = (binsB[0x8] > 0xf) ? 0 : maskB >> 4 * binsB[0x8];
    resultA = (resultA & ~maskA) | (maskA & 0x7777777777777777ul);
    maskA = (binsA[0x7] > 0xf) ? 0 : maskA >> 4 * binsA[0x7];
    resultB = (resultB & ~maskB) | (maskB & 0x7777777777777777ul);
    maskB = (binsB[0x7] > 0xf) ? 0 : maskB >> 4 * binsB[0x7];
    resultA = (resultA & ~maskA) | (maskA & 0x6666666666666666ul);
    maskA = (binsA[0x6] > 0xf) ? 0 : maskA >> 4 * binsA[0x6];
    resultB = (resultB & ~maskB) | (maskB & 0x6666666666666666ul);
    maskB = (binsB[0x6] > 0xf) ? 0 : maskB >> 4 * binsB[0x6];
    resultA = (resultA & ~maskA) | (maskA & 0x5555555555555555ul);
    maskA = (binsA[0x5] > 0xf) ? 0 : maskA >> 4 * binsA[0x5];
    resultB = (resultB & ~maskB) | (maskB & 0x5555555555555555ul);
    maskB = (binsB[0x5] > 0xf) ? 0 : maskB >> 4 * binsB[0x5];
    resultA = (resultA & ~maskA) | (maskA & 0x4444444444444444ul);
    maskA = (binsA[0x4] > 0xf) ? 0 : maskA >> 4 * binsA[0x4];
    resultB = (resultB & ~maskB) | (maskB & 0x4444444444444444ul);
    maskB = (binsB[0x4] > 0xf) ? 0 : maskB >> 4 * binsB[0x4];
    resultA = (resultA & ~maskA) | (maskA & 0x3333333333333333ul);
    maskA = (binsA[0x3] > 0xf) ? 0 : maskA >> 4 * binsA[0x3];
    resultB = (resultB & ~maskB) | (maskB & 0x3333333333333333ul);
    maskB = (binsB[0x3] > 0xf) ? 0 : maskB >> 4 * binsB[0x3];
    resultA = (resultA & ~maskA) | (maskA & 0x2222222222222222ul);
    maskA = (binsA[0x2] > 0xf) ? 0 : maskA >> 4 * binsA[0x2];
    resultB = (resultB & ~maskB) | (maskB & 0x2222222222222222ul);
    maskB = (binsB[0x2] > 0xf) ? 0 : maskB >> 4 * binsB[0x2];
    resultA = (resultA & ~maskA) | (maskA & 0x1111111111111111ul);
    maskA = (binsA[0x1] > 0xf) ? 0 : maskA >> 4 * binsA[0x1];
    resultB = (resultB & ~maskB) | (maskB & 0x1111111111111111ul);
    maskB = (binsB[0x1] > 0xf) ? 0 : maskB >> 4 * binsB[0x1];
    resultA = (resultA & ~maskA);
    resultB = (resultB & ~maskB);

    buf[i + 0] = resultA;
    buf[i + 1] = resultB;
  }
}
