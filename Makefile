DEP=test_filter_tree.c filter_tree.c filter_tree.h filters/bloom_filter.c

test_filter_tree: $(DEP)
	clang -g -o test_filter_tree -Wall -I./filters test_filter_tree.c filter_tree.c filters/bloom_filter.c filters/murmur3.c -lm

test_bloom_filter: $(DEP) test_bloom_filter.c
	clang -g -o test_bloom_filter -Wall -I./filters test_bloom_filter.c filter_tree.c filters/bloom_filter.c filters/murmur3.c -lm