#include <stdint.h>

#include "ksort.h"
#include "khash.h"

KSORT_INIT_GENERIC(uint32_t);

typedef struct {
    uint8_t* str; 
    uint16_t len;
} dstr_t;
static kh_inline khint_t __ac_X31_hash_dstring(const dstr_t dstr) {
    khint_t h = (khint_t) dstr.str[0];
    for (uint16_t i=1; i<dstr.len; ++i) h = (h << 5) - h + (khint_t) dstr.str[i];
    return h;
}
#define kh_dstr_hash_func(key) __ac_X31_hash_dstring(key)
#define kh_dstr_hash_equal(a, b) (memcmp(a.str, b.str, a.len) == 0)
// KHASH_MAP_INIT_STR(hmstr, uint32_t); // key -> int
KHASH_INIT(hmstr, dstr_t, uint32_t, 1, kh_dstr_hash_func, kh_dstr_hash_equal)


khash_t(hmstr)* seg_cnt;             // seg -> counter

 
void 
raw_line_parser(char *line, uint8_t k);

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
stat_print(uint8_t k);

void
free_keys();