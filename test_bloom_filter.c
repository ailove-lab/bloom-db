#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "filter.h"
#include "colors.h"

void test_probability(uint32_t c, float p);
uint64_t xorshift64star(uint64_t x);

int main(int argc, char** argv) {

    uint32_t size = 1<<20;
    test_probability(size, 0.50);
    test_probability(size, 0.40);
    test_probability(size, 0.30);
    test_probability(size, 0.20);
    test_probability(size, 0.10);
    test_probability(size, 0.05);
    test_probability(size, 0.01);
    
    return 0;
}

void test_probability(const uint32_t counter, float p) {

    struct filter_t* f = filter_new(counter, p);
    
    uint32_t i = 0;
    uint64_t h = 1;
    while(++i<=counter) {
        h = xorshift64star(h);
        filter_set(f,(char*) &h, 8);
    }
    filter_print(f);

    i = 0;
    uint32_t fp = 0;
    // h  = 2;
    while(++i<=counter) {
        h = xorshift64star(h);
        fp += filter_get(f, (char*)&h, 8) ? 1 : 0;
    }
    printf(KGRN "%u" RESET " / " KBLU "%u" RESET "\n", fp, counter);
    printf(KGRN "%f" RESET " / " KBLU "%f" RESET "\n", p, (float) fp / (float) counter);
    filter_free(f);

}

uint64_t xorshift64star(uint64_t x) {
    x ^= x >> 12; // a
    x ^= x << 25; // b
    x ^= x >> 27; // c
    return x * UINT64_C(2685821657736338717);
}
