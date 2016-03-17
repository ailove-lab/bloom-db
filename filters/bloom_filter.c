#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <math.h>

#include "filter.h"
#include "murmur3.h"

// TODO
// Huge filters over 4e9 bits needs uint64_t hashes!

#define bit_set(v,n)    ((v)[(n) >> 3] |= (0x1 << (0x7 - ((n) & 0x7))))
#define bit_get(v,n)    ((v)[(n) >> 3] &  (0x1 << (0x7 - ((n) & 0x7))))
#define bit_clr(v,n)    ((v)[(n) >> 3] &=~(0x1 << (0x7 - ((n) & 0x7))))

#define LN2 0.6931471805599453
#define LN4 0.4804530139182014

struct filter_bloom_t {
    struct filter_t filter;
    uint64_t bit_size;
    uint64_t byte_size;
    uint8_t  hash_num;
    uint64_t entries;
    uint8_t* data;
};

struct filter_t* filter_new(uint64_t size, float error) { 

    struct filter_bloom_t* filter = 
        (struct filter_bloom_t*) malloc(sizeof(struct filter_bloom_t));
    
    filter->filter.size  = size;
    filter->filter.error = error;
    filter->bit_size  = (uint64_t)ceil(-(float)size * log(error) / LN4);
    filter->byte_size = (uint64_t)ceil((float)filter->bit_size / 8.0);
    filter->hash_num  = (uint8_t)((float)filter->bit_size/(float)size * LN2);
    filter->entries = 0;

    filter->data = malloc(filter->byte_size);

    return (struct filter_t*) filter;

}

void filter_free(struct filter_t* filter) {

    if(filter == NULL) return;

    struct filter_bloom_t* f = (struct filter_bloom_t*) filter;
    static uint32_t count=0; count++;
    free(f->data);
    free(filter);
}

void filter_set(struct filter_t* filter, const char* key, size_t len) {

    if(filter == NULL) return;

    struct filter_bloom_t* f = (struct filter_bloom_t*) filter;

    uint32_t i;
    uint32_t h;

    for (i = 0; i < f->hash_num; i++) {
        murmur3_hash32(key, len, i, &h);
        h %= f->bit_size;
        bit_set(f->data, h);
    }
    f->entries++;

}

uint8_t filter_get(struct filter_t* filter, const char* key, size_t len) {

    struct filter_bloom_t* f = (struct filter_bloom_t*) filter;

    uint32_t i;
    uint32_t h;

    for (i = 0; i < f->hash_num; i++) {
        murmur3_hash32(key, len, i, &h);
        h %= f->bit_size;
        if (!bit_get(f->data, h))
            return 0;
    }
    return 1;
}

void filter_print(struct filter_t* filter) {
    
    if(filter == NULL) return;

    struct filter_bloom_t* f = (struct filter_bloom_t*) filter;
    printf("Filter @ %p\n", filter);
    printf("- filter -\n");
    printf("size:  %lu\n", filter->size);
    printf("error: %f\n", filter->error);
    printf("- bloom -\n");
    printf("bit_size:   %lu\n", f->bit_size);
    printf("bit/entry:  %f\n" , (float)f->bit_size/(float)filter->size);
    printf("byte_size:  %lu\n", f->byte_size);
    printf("hash_num:   %u\n" , f->hash_num);
    printf("entries:    %lu\n", f->entries);
}