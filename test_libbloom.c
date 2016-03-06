#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bloom.h"

#define STRS 24

static char* rand_string(char*, size_t);

int main(int argc, char** argv) {

    if (argc!=2) return printf("usage: test_libbloom entries\n");

    int entries = atoi(argv[1]);

    int seed = time(NULL);
    srand(seed);
    
    char str[STRS];
    struct bloom* bloom = (struct bloom*) calloc(1, sizeof(struct bloom));
    bloom_init(bloom, entries, 0.0001);
    for (int i = 0; i < entries; ++i) {
        rand_string(str, STRS);
        bloom_add(bloom, str, STRS);
        // printf("%s\n", str);
    }
    bloom_print(bloom);
    printf("%f\n", (float)bloom->collisions / (float)bloom->entries);
    bloom_free(bloom);
}

  ///////////////
 // FUNCTIONS //
///////////////

static char* rand_string(char* str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890+/";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}
