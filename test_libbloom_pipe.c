#include <stdio.h>
#include <stdlib.h>

#include "bloom.h"

#define SIZE 10000000
#define STRS 24

int main(int argc, char** argv) {

    if(argc != 2) return printf("set bloom size\n");

    struct bloom* bloom = (struct bloom*) calloc(1, sizeof(struct bloom));
    bloom_init(bloom, atoi(argv[1]), 0.0001);
    
    char* line = NULL;
    size_t len =0;
    while (getline(&line, &len, stdin) != -1) bloom_add(bloom, line, len-1);
    bloom_print(bloom);
    printf("%f\n", (float)bloom->collisions / (float)bloom->entries);
    bloom_free(bloom);
}
