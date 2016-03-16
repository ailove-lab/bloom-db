
#include <stdint.h>
#include <stddef.h>

struct filter_t {
    uint64_t size; // elements
    float error;   // false positive error
};

struct filter_t* filter_new(uint64_t size, float error);
void filter_print(struct filter_t* filter);
void filter_free(struct filter_t* filter);

void      filter_set(struct filter_t* filter, const char* key, size_t len);
uint8_t   filter_get(struct filter_t* filter, const char* key, size_t len);