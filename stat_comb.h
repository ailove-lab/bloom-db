#include <stdint.h>

#include "ksort.h"
#include "khash.h"

union comb_key {
    uint64_t seg64[1];
    uint32_t seg32[2];
    char     seg8 [8];
};

KSORT_INIT_GENERIC(uint32_t);

KHASH_MAP_INIT_INT64(hm64, uint32_t); // key -> int
khash_t(hm64)* seg_cnt;               // seg -> counter

 
void 
raw_line_parser(char *line);

void 
cnk_list(uint32_t arr[], uint32_t n, uint32_t k, 
         uint32_t** results, uint32_t* combinations);

void 
cnk_until(
    uint32_t arr[],    uint32_t data[], 
    uint32_t start,    uint32_t end, 
    uint32_t index,    uint32_t r, 
    uint32_t* results, uint32_t* combinations);

uint32_t 
cnk_count(uint32_t n, uint32_t k );

void 
stat_print();