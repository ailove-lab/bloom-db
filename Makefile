DEP=filter_tree.c filter_tree.h filters/bloom_filter.c filters/filter.h colors.h
CF=-Wall -O3
I=-I./ -I./filters -I./lib/klib 
DEP=filter_tree.c filters/bloom_filter.c timer.c filters/murmur3.c -lm

stat_comb: stat_comb.c stat_comb.h
	clang $(CF) -o $@ $(I) $@.c 

test_fill_tree: $(DEP) test_fill_tree.c
	clang -o $@ $(CF) $(I) $@.c $(DEP)

test_filter_tree: $(DEP) test_filter_tree.c
	clang -o $@ $(CF) $(I) $@.c $(DEP)

test_bloom_filter: $(DEP) test_bloom_filter.c
	clang -o $@ $(CF) $(I) $@.c $(DEP)

test_errors: test_errors.c colors.h
	clang -Wall -O3 -o $@ $@.c 