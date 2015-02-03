typedef unsigned long size_t;

#include <stdint.h>

#ifndef NOSIMD
void __attribute__((target("arch=haswell")))
nibble_sort_alexander1(unsigned long *buf);
void __attribute__((target("arch=haswell")))
nibble_sort_alexander3(unsigned long *buf);
void nibble_sort_alexander4(unsigned long *buf);
#endif
void nibble_sort_alexander2(unsigned long *buf);
void nibble_sort_anon(unsigned long *buf);
void nibble_sort_uwe(unsigned long *buf);
void nibble_sort_arseny1(uint64_t *buf);
void nibble_sort_arseny2(uint64_t *buf);
void nibble_sort_beekman1(uint64_t *buf);
void nibble_sort_beekman2(uint64_t *buf);
void nibble_sort_bloom(unsigned long *buf);
void nibble_sort_burton1(unsigned long *buf);
void nibble_sort_burton2(unsigned long *buf);
void nibble_sort_carlos(long *buf);
void nibble_sort_chucky(unsigned long *buf);
void nibble_sort_jarkko(unsigned long *buf);
void nibble_sort_jepler(uint64_t *buf);
void nibble_sort_joris(unsigned long *buf);
void nibble_sort_justin(unsigned long *buf);
void nibble_sort_mats(unsigned long *buf);
void nibble_sort_mikael(uint64_t *buf);
void nibble_sort_mike(unsigned long *buf);
void nibble_sort_nadav(unsigned long *buf);
void nibble_sort_parnell(unsigned long *buf);
void nibble_sort_payer(unsigned long *buf);
void nibble_sort_pdewacht1(unsigned long *buf);
void nibble_sort_pdewacht2(unsigned long *buf);
void nibble_sort_ref(unsigned long *buf);
void nibble_sort_robin(unsigned long *buf);
void nibble_sort_rogers(uint64_t *buf);
void nibble_sort_rosen(unsigned long *pv);
void nibble_sort_scotty(unsigned long *buf);
void nibble_sort_tom(unsigned long *buf);
void nibble_sort_vetter1(unsigned long long *buf);
void nibble_sort_vetter2(unsigned long long *buf);
void nibble_sort_vetter3(unsigned long long *buf);
void nibble_sort_vetter4(unsigned long long *buf);
void nibble_sort_vlad(unsigned long *pv);
void nibble_sort_grayson(unsigned long *buf);
void nibble_sort_falk(unsigned long *buf);
void nibble_sort_karim(uint64_t *buf);
void nibble_sort_jrose(unsigned long buf[static 1024]);
void nibble_sort_rjk9(unsigned long *buffer);
void nibble_sort_frank(unsigned long *buf);
void nibble_sort_koorogi1(unsigned long *buf);
void nibble_sort_koorogi2(unsigned long *buf);
void nibble_sort_hans(uint64_t *buf);
void nibble_sort_bosmans1(unsigned long *buf);
void nibble_sort_bosmans2(unsigned long *buf);
void nibble_sort_mentre(unsigned long *buf);
void nibble_sort_jerome(uint64_t *buf);
