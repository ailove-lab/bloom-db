#!/bin/bash

cf="-Ofast -Wall -D __linux__"
lf="-pthread -lm"

bloom_pth="./lib/libbloom"
bloom_src="$bloom_pth/bloom.c $bloom_pth/linux.c $bloom_pth/murmur2/MurmurHash2.c"
bloom_inc="-I$bloom_pth -I$bloom_pth/murmur2"

klib_pth="./lib/klib"
klib_inc="-I$klib_pth"

clang $cf -o test_libbloom $bloom_inc \
test_libbloom.c $bloom_src $lf

clang $cf -o test_libbloom_pipe $bloom_inc \
test_libbloom_pipe.c $bloom_src $lf