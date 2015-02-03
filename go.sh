#!/bin/bash

# first: turn off CPU frequency scaling
# disable turbo boost?

set -e

CC=gcc
OPT=-O3

# $CC -v

rm -f test_nibble *.o

if [ "$1" = "NOSIMD" ]; then
  SIMD=-DNOSIMD
  CMD="$CC -std=gnu99 $SIMD $OPT -c -mno-avx -mno-mmx -mno-sse"
  $CMD nadav.c
  $CMD anon.c
  $CMD -fprefetch-loop-arrays falk.c
  $CMD vlad.c
  $CMD hans.c
  $CMD mentre.c
  $CMD grayson.c
  $CMD payer.c
  $CMD frank.c
  $CMD jerome.c
  $CMD bosmans2.c
  $CMD rjk9.c
  $CMD jrose.c
  $CC -c -std=gnu99 $OPT -funroll-loops pdewacht1.c
  $CMD ref.c
  $CMD burton1.c
  $CMD burton2.c
  $CMD karim.c
  $CMD mats.c
  $CMD robin.c
  $CMD jepler.c
  $CMD mike.c
  $CMD chucky.c
  $CMD bloom.c
  $CMD rogers.c
  $CMD joris.c
  $CMD justin.c
  $CMD vetter1.c
  $CMD vetter2.c
  $CMD vetter3.c
  $CMD vetter4.c
  $CMD rosen.c
  $CMD jarkko.c
  $CMD parnell.c
  $CMD alexander2.c
else
  CMD="$CC -std=gnu99 $OPT -c"
  $CMD alexander1.c
  $CMD alexander2.c
  $CMD alexander3.c
  $CMD alexander4.c
  $CMD grayson.c
  $CMD rjk9.c
  $CMD vlad.c
  $CMD hans.c
  $CMD mentre.c
  $CMD -march=core-avx2 jerome.c
  $CMD -march=native bosmans1.c
  $CMD -march=native bosmans2.c
  $CMD frank.c
  $CMD nadav.c
  $CMD jrose.c
  $CMD karim.c
  $CMD -mavx koorogi1.c
  $CMD -mavx koorogi2.c
  $CMD -march=core-avx2 uwe.c
  $CMD -march=native -fprefetch-loop-arrays falk.c
  $CMD anon.c
  $CC -std=gnu99 $OPT -c -march=core-avx2 carlos.c
  $CMD payer.c
  $CC -c -std=gnu99 $OPT -march=haswell -funroll-loops pdewacht1.c
  $CC -c -std=gnu99 $OPT -march=haswell -funroll-loops pdewacht2.c
  $CMD -mavx2 beekman1.c
  $CMD -mavx2 beekman2.c
  $CC -c -std=gnu99 $OPT -mavx2 arseny1.c
  $CC -c -std=gnu99 $OPT -mavx2 arseny2.c
  $CMD ref.c
  $CMD burton1.c
  $CMD burton2.c
  $CMD mats.c
  $CMD robin.c
  $CMD jepler.c
  $CMD mike.c
  $CMD chucky.c
  $CMD bloom.c
  $CMD rogers.c
  $CMD joris.c
  $CMD justin.c
  $CMD vetter1.c
  $CMD vetter2.c
  $CMD vetter3.c
  $CMD vetter4.c
  $CC -c -std=gnu99 -march=native -fbranch-probabilities -ftracer -fprefetch-loop-arrays $OPT scotty.c
  $CMD rosen.c
  $CMD jarkko.c
  $CMD parnell.c
  $CC -c -std=gnu99 $OPT -mavx tom.c
  $CMD -std=gnu99 $OPT -msse4.1 -c mikael.c
fi

$CC $SIMD -O3 -std=gnu99 -c test_nibble.c
#$CC *.o -o test_nibble -lrt
$CC *.o -o test_nibble 

# taskset -c 0 ./test_nibble | sort -n
./test_nibble | sort -n
