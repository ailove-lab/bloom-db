
#include <stdint.h>
#include <stddef.h>

#include "filter.h"

// tree
struct ft_trunk_t;
struct ft_trunk_t {
    struct filter_t* filter;
    struct ft_trunk_t* branches[2];
    uint8_t is_leaf;
    uint32_t id;
};

// create new trunk
struct ft_trunk_t* ft_new_trunk();

// initialize filter size 
void ft_init_filter(struct ft_trunk_t* trunk, uint32_t size, float err);

// add  branches
void ft_grow(struct ft_trunk_t* trunk);

// check trunk's filter for key
uint8_t ft_get_key(struct ft_trunk_t*, char* key, uint8_t len);

// write key to trunk's filter
uint8_t ft_set_key(struct ft_trunk_t*, char* key, uint8_t len);
