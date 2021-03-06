
#include <stdint.h>
#include <stddef.h>

#include "filter.h"
#include "khash.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#define MAX_STAT_SIZE  4096
#define FILTER_ERROR_1 0.40
#define FILTER_ERROR_2 0.01

#include "colors.h"

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

KHASH_MAP_INIT_INT(i32, struct ft_trunk_t*);

// tree
struct ft_tree_t {
    struct ft_trunk_t* root;  // root trunk
    uint32_t* buf_names;      // list of leafs names
    uint64_t* buf_integral;   // list of integrals of bloom entries
    uint16_t  buf_size;       // size of lists
    khash_t(i32)* index;
};

// grows tree by list of leafs
// names      - names list
// leaf_sizes - leaf filter sizes list
// len        - len of list
struct ft_trunk_t* ft_grow_list(uint32_t* buf_names, uint64_t* buf_integral, uint16_t len, struct ft_trunk_t** stack, uint8_t stack_size);

// grows tree by file with statistic
struct ft_tree_t* ft_grow_file(char* filename);

// recursively check all trunks
void ft_get_leaf(struct ft_trunk_t* trunk, char* key, uint32_t* results, uint8_t* results_size);

// check trunk's filter for key
void ft_get_key(struct ft_tree_t* trunk, char* key, uint32_t* results, uint8_t* results_size);

// write key to trunk's filter
uint8_t ft_set_key(struct ft_tree_t* trunk, char* key, uint32_t* values, uint8_t values_size);

// fill tree with data from a file
uint8_t ft_fill_tree(struct ft_tree_t* tree, char* filename);

// build leafs index leaf_name -> trunk
void ft_build_index(struct ft_trunk_t* trunk, khash_t(i32)* hashmap);

// debug print tree
void ft_print_tree(struct ft_tree_t* tree);

// debug print trunk
void ft_print_trunk(struct ft_trunk_t* trunk);

// cleanup trunk
void ft_free_trunk(struct ft_trunk_t* trunk);

// cleanup tree
void ft_free_tree(struct ft_tree_t* tree);
