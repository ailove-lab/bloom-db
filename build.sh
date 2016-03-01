#!/bin/bash

cf="-Ofast -Wall -D __linux__"
lf="-pthread -lm"

bloom_pth="./lib/libbloom"
bloom_src="$bloom_pth/bloom.c $bloom_pth/linux.c $bloom_pth/murmur2/MurmurHash2.c"
bloom_inc="-I$bloom_pth -I$bloom_pth/murmur2"

klib_pth="./lib/klib"
klib_inc="-I$klib_pth"

clang $cf -o stat-pip $klib_inc stat.c

clang $cf -o indexer $bloom_inc $klib_inc \
indexer.c timer.c \
$bloom_src $klib_pth/kthread.c $lf

clang $cf -o query $bloom_inc $klib_inc \
query.c timer.c \
$bloom_src $klib_pth/kthread.c $lf

clang $cf -o info $bloom_inc \
info.c $bloom_src $lf