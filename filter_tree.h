
#include <stdint.h>
#include <stddef.h>

#include "filter.h"

#define MAX_STAT_SIZE 4096
#define FILTER_ERROR 0.5

// trunk
struct ft_trunk_t;
struct ft_trunk_t {
    struct filter_t* filter;
    struct ft_trunk_t* branches[2];
    uint8_t is_leaf;
    uint32_t leaf_name;
    struct ft_trunk_t** parents;
    uint8_t parents_size;
};

// tree
struct ft_tree_t {
    struct ft_trunk_t* root;
    uint32_t* buf_names;
    uint64_t* buf_integral;
};

// grows tree by list of leafs
// names      - names list
// leaf_sizes - leaf filter sizes list
// len        - len of list
struct ft_trunk_t* ft_grow_list(uint32_t* buf_names, uint64_t* buf_integral, uint16_t len, struct ft_trunk_t** stack, uint8_t stack_size);

// grows tree by file with statistic
struct ft_tree_t* ft_grow_file(char* filename);

// // check trunk's filter for key
// uint8_t ft_get_key(struct ft_trunk_t* trunk, char* key, uint8_t len);
// 
// // write key to trunk's filter
// uint8_t ft_set_key(struct ft_trunk_t* trunk, char* key, uint8_t len);

// debug print
void ft_print(struct ft_trunk_t* trunk);

// cleanup trunk
void ft_free_trunk(struct ft_trunk_t* trunk);

// cleanup tree
void ft_free_tree(struct ft_tree_t* tree);