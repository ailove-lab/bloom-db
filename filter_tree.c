#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "filter_tree.h"

struct ft_trunk_t* 
ft_grow_list(uint32_t* buf_names, uint64_t* buf_integral, uint16_t len, struct ft_trunk_t** stack, uint8_t stack_size){
    
    struct ft_trunk_t* trunk = (struct ft_trunk_t*) malloc(sizeof(struct ft_trunk_t));
    stack[stack_size++] = trunk;

    uint64_t size = buf_integral[len]-buf_integral[0];
    trunk->filter = filter_new(size, FILTER_ERROR);
    
    if(len == 1) {
    
        trunk->is_leaf   = 1;
        trunk->leaf_name = buf_names[0];
        trunk->branches[0]=NULL;
        trunk->branches[1]=NULL;
        trunk->parents = (struct ft_trunk_t**) malloc(stack_size * sizeof(struct ft_trunk_t*));
        trunk->parents_size = stack_size-1;
        memcpy(trunk->parents, stack, (stack_size-1) * sizeof(struct ft_trunk_t*));
    
    } else {
    
        trunk->is_leaf      = 0;
        trunk->leaf_name    = 0;
        trunk->parents      = NULL;
        trunk->parents_size = 0;

        uint16_t half = len>>1;
        if(!half) half = 1;
        trunk->branches[0] = ft_grow_list(buf_names     , buf_integral     ,     half, stack, stack_size);
        trunk->branches[1] = ft_grow_list(buf_names+half, buf_integral+half, len-half, stack, stack_size);
    }
    stack_size--;
    return trunk;
}


// grows tree by file with statistic
struct ft_tree_t* 
ft_grow_file(char* filename) {

    struct ft_tree_t* tree = (struct ft_tree_t*) malloc(sizeof(struct ft_tree_t)); 
    tree->buf_names    = (uint32_t*) malloc(MAX_STAT_SIZE * sizeof(uint32_t));
    tree->buf_integral = (uint64_t*) malloc(MAX_STAT_SIZE * sizeof(uint64_t));

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    size_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) return NULL;

    uint16_t i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        char *cnt = strchr(line, ' ');
        if (cnt == NULL) continue;
        *cnt = 0; cnt++;
        tree->buf_names[i] = atol(line);
        tree->buf_integral[i+1] = tree->buf_integral[i] + atol(cnt);
        i++;
        if(i>=MAX_STAT_SIZE-1) break;
    }

    fclose(fp);
    if (line) free(line);
    struct ft_trunk_t* stack[256];
    tree->root = ft_grow_list(tree->buf_names, tree->buf_integral, i, stack, 0);
    // tree->index = ft_build_index(tree->root);

    return tree;
}

// khash_t(ui32)*
// ft_build_index(struct ft_trunk_t* trunk) {
// 
// }
// 

// check tree for key
// uint8_t 
// ft_get_key(struct ft_tree_t* tree, char* key, uint16_t* values, uint8_t values_count) {
//     
//     // get hashes
//     // run through branches 
//     // return names of leafs with positive response
// 
//     return 0;
// }

// write key to tree
// uint8_t 
// ft_set_key(struct ft_tree_t* trunk, char* key, uint16_t* values, uint8_t values_count) {
//     // iterate through leafs
//     // get leaf`s parent trunks
//     // set filters of parents and leaf
// 
//     return 0;
// }

// print trunk and branches
void 
ft_print(struct ft_trunk_t* trunk) {
    
    #define KNRM  "\x1B[0m"
    #define KRED  "\x1B[31m"
    #define KGRN  "\x1B[32m"
    #define KYEL  "\x1B[33m"
    #define KBLU  "\x1B[34m"
    #define KMAG  "\x1B[35m"
    #define KCYN  "\x1B[36m"
    #define KWHT  "\x1B[37m"
    #define RESET "\033[0m"
    
    if(trunk == NULL) return;

    static uint16_t level = 0;
    level++;
    
    for(uint16_t i=0; i<level-1; i++) printf("..");
    if(trunk->is_leaf) {
        printf(KGRN "%p" RESET "\t" KMAG "%u " RESET KCYN "%lu" RESET " [", 
            trunk, trunk->leaf_name, trunk->filter->size);
        // filter_print(trunk->filter);
        for(uint8_t j=0; j<trunk->parents_size; j++) printf("%p ", trunk->parents[j]);
        printf("]\n");
    } else {
        printf(KBLU "%p\n" RESET, trunk);
    }

    if(trunk == NULL) return;
    if(!trunk->is_leaf) {
        ft_print(trunk->branches[0]);
        ft_print(trunk->branches[1]);
    }
    level--;
}

// clean up trunk
void 
ft_free_trunk(struct ft_trunk_t* trunk) {
    
    if(trunk == NULL) return;

    filter_free(trunk->filter);
    if(trunk->is_leaf) {
        free(trunk->parents);
    } else {
        ft_free_trunk(trunk->branches[0]);
        ft_free_trunk(trunk->branches[1]);
    } 
    free(trunk);
}

// cleanup tree
void 
ft_free_tree(struct ft_tree_t* tree) {
    
    if(tree == NULL) return;

    ft_free_trunk(tree->root);

    free(tree->buf_names);
    free(tree->buf_integral);
    free(tree);
}
