
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "stat_comb.h"
#include "colors.h"

int main(int argc, char** argv) {

    if(argc!=2) return -1;
    
    seg_cnt = kh_init(hmstr);

    char *line = NULL;
    size_t len = 0;
    uint8_t k = atoi(argv[1]);
    while (getline(&line, &len, stdin) != -1) raw_line_parser(line, k);
    free(line);

    stat_print(k);
    
    free_keys();
    kh_destroy(hmstr, seg_cnt);
    return 0;

}
 
void 
raw_line_parser(char *line, uint8_t k) {
    
    char *seg, *tab, *end;
    
    end = strchr(line, '\n');     // find end / new line
    if(end == NULL) return;       
    *end = 0;                     // replace it fith terminator

    tab = strchr(line, '\t');     // find tab
    if(tab == NULL) return;
    *tab = 0;                    // split key/data
    // get last space
    seg  = strrchr(tab+1, ' ');  // find segments
    if(seg == NULL) return;
    *seg = 0;                    // data to flags/segments
    seg++;

    // iterate through segments
    char *s, *sv;
    uint32_t segs[256] = {0};
    uint8_t segs_count = 0;
    s = strtok_r(seg, "/", &sv);
    while (s!=NULL) {
        segs[segs_count++] = atol(s);
        // next token
        s = strtok_r(NULL, "/", &sv);
    }

    // printf("segments: ");
    // for(uint8_t i=0; i<segs_count; i++) printf(KBLU"%08x ", segs[i]); printf(RESET"\n");
    // ks_introsort(uint32_t, segs_count, segs);
    // printf("sorted:   ");
    // for(uint8_t i=0; i<segs_count; i++) printf(KMAG"%08x ", segs[i]); printf(RESET"\n");

    uint32_t combinations = 0;
    uint32_t* results = NULL;
    cnk_list(segs, segs_count, k, &results, &combinations);

    // printf("combinations: "KRED"%u\n"RESET, combinations);
    // for(uint32_t i=0; i<combinations; i++) {
    //     printf("%03u: ", i+1);
    //     for(uint8_t j=0; j<k; j++)
    //         printf(KGRN"%08x "RESET, results[k*i+j]);
    //     printf("\n");
    // }

    for(uint32_t i=0; i<combinations; i++) {
        uint16_t len = k*sizeof(uint32_t);
        uint8_t* key = (uint8_t*) calloc(len, sizeof(char));
        memcpy(key, &results[i*k], len);
        dstr_t dstr = {key, len};

        // print bytes of key
        // for(uint8_t j=0; j<len; j++) printf("%02x ", key[j]); printf("\n");
        
        int ret;
        khiter_t ki;
        ki = kh_get(hmstr, seg_cnt, dstr);
        if(ki == kh_end(seg_cnt)) {
            ki = kh_put(hmstr, seg_cnt, dstr, &ret);
            kh_value(seg_cnt, ki) = 1;
        } else {
            kh_value(seg_cnt, ki) += 1;
        }
    }
}


void
cnk_list(
    uint32_t arr[], uint32_t n, uint32_t k, 
    uint32_t** results, uint32_t* combinations){
    *combinations = 0;
    *results = (uint32_t*) calloc(cnk_count(n, k) * k, sizeof(uint32_t));
    uint32_t data[k];
    cnk_until(arr, data, 0, n-1, 0, k, *results, combinations);
}

void 
cnk_until(
    uint32_t arr[],    uint32_t data[], 
    uint32_t start,    uint32_t end, 
    uint32_t index,    uint32_t k, 
    uint32_t* results, uint32_t* combinations) {

    if (index == k) {
        for (uint32_t j=0; j<k; j++) results[(*combinations)*k+j] = data[j];
        (*combinations)++;
        return;
    }
    
    for (int i=start; i<=end && end-i+1 >= k-index; i++) {
        data[index] = arr[i];
        cnk_until(arr, data, i+1, end, index+1, k, results, combinations);
    }
}

uint32_t 
cnk_count(uint32_t n, uint32_t k ) {
    
    if (k > n) return 0;
    if (k * 2 > n) /*return*/ k = n-k;  //remove the commented section
    if (k == 0) return 1;
 
    uint32_t result = n;
    for( uint32_t i = 2; i <= k; ++i ) {
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

// Print statistic
void 
stat_print(uint8_t k) {
    // segment counters
    // #define KBLU  ""
    // #define RESET ""
    // #define KGRN  ""
    for (khiter_t ki=kh_begin(seg_cnt); ki!=kh_end(seg_cnt); ++ki) {
        if (kh_exist(seg_cnt, ki)) {
            dstr_t dstr = (dstr_t) kh_key(seg_cnt, ki);
            uint32_t cnt = kh_value(seg_cnt, ki);
            // if(cnt < 100) continue;
            for(uint8_t i=0; i<k; i++) {
                printf(KBLU"%8d "RESET, *(uint32_t*)&(dstr.str[i*sizeof(uint32_t)]));
            }
            printf(KGRN"%d"RESET"\n", cnt);
        }
    }
}

// free keys
void 
free_keys() {
    // segment counters
    for (khiter_t ki=kh_begin(seg_cnt); ki!=kh_end(seg_cnt); ++ki) {
        if (kh_exist(seg_cnt, ki)) {
            free(kh_key(seg_cnt, ki).str);
        }
    }
}