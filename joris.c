#include "nibble.h"

void nibble_sort_joris(unsigned long *buf) {
  for (int i = 0; i < 256; i++) {

    unsigned long da = buf[0], db = buf[1], dc = buf[2], dd = buf[3];
    unsigned long ca = 0, cb = 0, cc = 0, cd = 0;

    for (int k = 0; k < 16; k++) {
      ca += 1UL << ((da & 15) << 2);
      cb += 1UL << ((db & 15) << 2);
      cc += 1UL << ((dc & 15) << 2);
      cd += 1UL << ((dd & 15) << 2);
      da = (da >> 4) | (da << 60);
      db = (db >> 4) | (db << 60);
      dc = (dc >> 4) | (dc << 60);
      dd = (dd >> 4) | (dd << 60);
    }

    if ((ca & (ca - 1)) == 0)
      ca = 0;
    if ((cb & (cb - 1)) == 0)
      cb = 0;
    if ((cc & (cc - 1)) == 0)
      cc = 0;
    if ((cd & (cd - 1)) == 0)
      cd = 0;

    unsigned long kk = 0;
    for (int k = 16; k > 0; k--) {
      unsigned char ta = (ca & 15) << 2;
      da = (da >> ta) | (kk << (ta ^ 60));
      unsigned char tb = (cb & 15) << 2;
      db = (db >> tb) | (kk << (tb ^ 60));
      unsigned char tc = (cc & 15) << 2;
      dc = (dc >> tc) | (kk << (tc ^ 60));
      unsigned char td = (cd & 15) << 2;
      dd = (dd >> td) | (kk << (td ^ 60));
      ca >>= 4;
      cb >>= 4;
      cc >>= 4;
      cd >>= 4;
      kk += 0x1111111111111110;
    }

    buf[0] = da;
    buf[1] = db;
    buf[2] = dc;
    buf[3] = dd;

    buf += 4;
  }
}
