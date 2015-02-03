#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "nibble.h"

static long timediff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000L + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return temp.tv_sec * 1000000000L + temp.tv_nsec;
}

static const int REPS = 50;
static const int BUFSIZE = 1024;

typedef void (*nibble_func_t)(unsigned long *buf);
typedef struct {
  nibble_func_t func;
  const char *name;
} nibble_sort_t;
static nibble_sort_t funcs[] = {
#ifndef NOSIMD
    {nibble_sort_alexander1, "alexander1"},
    {nibble_sort_alexander3, "alexander3"},
    {nibble_sort_alexander4, "alexander4"},
    {nibble_sort_beekman1, "beekman1"},
    {nibble_sort_beekman2, "beekman2"},
    {nibble_sort_arseny1, "arseny1"},
    {nibble_sort_arseny2, "arseny2"},
    {nibble_sort_scotty, "scotty"},
    {nibble_sort_pdewacht2, "pdewacht2"},
    {nibble_sort_mikael, "mikael"},
    {(nibble_func_t)nibble_sort_carlos, "carlos"},
    {nibble_sort_tom, "tom"},
    {nibble_sort_koorogi1, "koorogi1"},
    {nibble_sort_koorogi2, "koorogi2"},
    {nibble_sort_uwe, "uwe"}, // permission = yes
    {nibble_sort_bosmans1, "bosmans1"},
#endif
    {nibble_sort_alexander2, "alexander2"},
    {nibble_sort_bosmans2, "bosmans2"},
    {nibble_sort_ref, "ref"},
    {nibble_sort_vlad, "vlad"},
    {nibble_sort_payer, "payer"},
    {nibble_sort_anon, "anon"},
    {nibble_sort_pdewacht1, "pdewacht1"},
    {nibble_sort_burton1, "burton1"},
    {nibble_sort_burton2, "burton2"},
    {nibble_sort_rogers, "rogers"},
    {nibble_sort_karim, "karim"},
    {nibble_sort_jepler, "jepler"},
    {nibble_sort_grayson, "grayson"},
    {(nibble_func_t)nibble_sort_vetter1, "vetter1"},
    {(nibble_func_t)nibble_sort_vetter2, "vetter2"},
    {(nibble_func_t)nibble_sort_vetter3, "vetter3"},
    {(nibble_func_t)nibble_sort_vetter4, "vetter4"},
    {nibble_sort_rosen, "rosen"},
    {nibble_sort_justin, "justin"},
    {nibble_sort_mats, "mats"},
    {nibble_sort_mike, "mike"},
    {nibble_sort_jarkko, "jarkko"},
    {nibble_sort_parnell, "parnell"},
    {nibble_sort_chucky, "chucky"},
    {nibble_sort_joris, "joris"},
    {nibble_sort_bloom, "bloom"},
    {nibble_sort_robin, "robin"},
    {nibble_sort_mentre, "mentre"},
    {nibble_sort_nadav, "nadav"},
    {nibble_sort_jerome, "jerome"},
    {nibble_sort_falk, "falk"},
    {nibble_sort_rjk9, "rjk9"},
    {nibble_sort_frank, "frank"},
    {nibble_sort_jrose, "jrose"},
    {nibble_sort_hans, "hans"},
};
#define NFUNCS (sizeof(funcs) / sizeof(nibble_sort_t))

int errors[NFUNCS];

static int rand_int(int n) {
  int limit = RAND_MAX - RAND_MAX % n;
  int rnd;
  do {
    rnd = rand();
  } while (rnd >= limit);
  return rnd % n;
}

// from:
// http://stackoverflow.com/questions/3343797/is-this-c-implementation-of-fisher-yates-shuffle-correct
static void shuffle(nibble_sort_t *array, int n) {
  for (int i = n - 1; i > 0; i--) {
    int j = rand_int(i + 1);
    nibble_sort_t tmp = array[j];
    array[j] = array[i];
    array[i] = tmp;
  }
}

static unsigned long *getbuf(void) {
  unsigned long *p;
  int res = posix_memalign((void **)&p, 4096, BUFSIZE * 8);
  assert(res == 0);
  assert(p);
  assert(((intptr_t)p & 0xfff) == 0);
  return p;
}

static void validate(unsigned long *test_data) {
  unsigned long *buf = getbuf();
  unsigned long *buf2 = getbuf();
  memcpy(buf2, test_data, BUFSIZE * 8);
  nibble_sort_ref(buf2);
  for (int func = 0; func < NFUNCS; ++func) {
    memcpy(buf, test_data, BUFSIZE * 8);
    funcs[func].func(buf);
    for (int i = 0; i < BUFSIZE; ++i) {
      if (buf[i] != buf2[i]) {
        if (0)
          printf("%s: expected %016lx at %d to sort to %016lx but got %016lx\n",
                 funcs[func].name, test_data[i], i, buf2[i], buf[i]);
        errors[func]++;
      }
    }
  }
  free(buf);
  free(buf2);
}

static void timing(unsigned long *test_data) {
  unsigned long *buf = getbuf();
  printf("%16s %16s %16s %16s\n", "ns", "ns / 8 bytes", "entry name", "errors");
  for (int func = 0; func < NFUNCS; ++func) {
    long times[REPS];
    long best = LONG_MAX;
    for (int i = 0; i < REPS; ++i) {
      memcpy(buf, test_data, BUFSIZE * 8);
      struct timespec start, end;
      clock_gettime(CLOCK_MONOTONIC, &start);
      funcs[func].func(buf);
      clock_gettime(CLOCK_MONOTONIC, &end);
      long t = timediff(start, end);
      times[i] = t;
      if (t < best)
        best = t;
    }
    double d = (double)best / BUFSIZE;
    printf("%16ld %16.2lf %16s %16d\n", best, d, funcs[func].name,
           errors[func]);
    if (0) {
      for (int i = 0; i < REPS; ++i)
        printf("%ld ", times[i]);
      printf("\n");
    }
  }
  free(buf);
}

int main(void) {
  assert(sizeof(unsigned long) == 8);
  srand(time(NULL) + getpid());

  shuffle(funcs, NFUNCS);

  unsigned long *pathological_data = getbuf();
  for (int i = 0; i < BUFSIZE; ++i)
    for (int j = 0; j < 8; ++j)
      ((unsigned char *)&pathological_data[i])[j] = i % 1024;
  validate(pathological_data);
  free(pathological_data);

  unsigned long *random_data = getbuf();
  for (int i = 0; i < BUFSIZE * 8; ++i)
    ((unsigned char *)random_data)[i] = rand() & 0xff;
  validate(random_data);
  timing(random_data);
  free(random_data);

  return 0;
}
