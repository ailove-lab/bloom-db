#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "filter_tree.h"

struct ft_trunk_t* 
ft_grow_list(uint32_t* buf_names, uint64_t* buf_integral, uint16_t len, struct ft_trunk_t** stack, uint8_t stack_size){
    
    struct ft_trunk_t* trunk = (struct ft_trunk_t*) calloc(1, sizeof(struct ft_trunk_t));
    stack[stack_size++] = trunk;

    uint64_t size = buf_integral[len]-buf_integral[0];
    trunk->filter = filter_new(size, FILTER_ERROR);
    
    if(len == 1) {
    
        trunk->is_leaf   = 1;
        trunk->leaf_name = buf_names[0];
        trunk->branches[0]=NULL;
        trunk->branches[1]=NULL;
        trunk->parents = (struct ft_trunk_t**) calloc(stack_size, sizeof(struct ft_trunk_t*));
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

    struct ft_tree_t* tree = 
        (struct ft_tree_t*) malloc(sizeof(struct ft_tree_t)); 
    
    tree->buf_names    = (uint32_t*) calloc(MAX_STAT_SIZE, sizeof(uint32_t));
    tree->buf_integral = (uint64_t*) calloc(MAX_STAT_SIZE, sizeof(uint64_t));

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
    tree->buf_size = i;

    // cleanup
    fclose(fp);
    if (line) free(line);

    struct ft_trunk_t* stack[256];
    tree->root  = ft_grow_list(tree->buf_names, tree->buf_integral, tree->buf_size, stack, 0);
    tree->index = kh_init(i32);
    ft_build_index(tree->root, tree->index);

    return tree;
}

// build leafs index leaf_name -> trunk
void
ft_build_index(struct ft_trunk_t* trunk, khash_t(i32)* hashmap) {
    
    if(trunk == NULL || hashmap == NULL) return;

    if(trunk->is_leaf) {
        
        int ret = 0; 

        khiter_t k;
        k = kh_put(i32, hashmap, trunk->leaf_name, &ret);

        // Return code: 
        // -1 if the operation failed; 
        //  0 if the key is present in the hash table;
        //  1 if the bucket is empty (never used); 
        //  2 if the element in the bucket has been deleted.
        if(ret == 1) kh_value(hashmap, k) = trunk;

    } else {
        ft_build_index(trunk->branches[0], hashmap);
        ft_build_index(trunk->branches[1], hashmap);
    }

}

// recursively check all trunks
void 
ft_get_leaf(struct ft_trunk_t* trunk, 
            char*              key, 
            uint32_t*          results, 
            uint8_t*           results_size) {

    static uint8_t depth = 0;
    depth++;
    uint8_t f = filter_get(trunk->filter, key, strlen(key));
    if(DEBUG) {
        for(uint8_t i=0;i<depth-1; i++) printf("..");
        printf("%s%p" RESET " = %d\n", trunk->is_leaf ? KGRN : KBLU, trunk->filter,  f);
    }
    
    if(f) {
        if (trunk->is_leaf) {
            results[(*results_size)++] = trunk->leaf_name;
        } else {
            ft_get_leaf(trunk->branches[0], key, results, results_size);
            ft_get_leaf(trunk->branches[1], key, results, results_size);
        }
    }
    depth--;

}

// check tree for key
void 
ft_get_key(struct ft_tree_t* tree, 
           char*             key, 
           uint32_t*         results, 
           uint8_t*          results_size) {

    *results_size = 0;
    if(DEBUG) printf("get %s\n", key);
    ft_get_leaf(tree->root, key, results, results_size);
}


// write key to tree
uint8_t 
ft_set_key(struct ft_tree_t* tree, 
           char*             key, 
           uint32_t*         values, 
           uint8_t           values_count) {
    
    khiter_t k;
    struct ft_trunk_t* leaf  = NULL;
    struct ft_trunk_t* trunk = NULL;

    for(uint8_t i=0; i<values_count; i++) {
        k = kh_get(i32, tree->index, values[i]);
        if(k == kh_end(tree->index)) {
            printf(KRED "NO SUCH LEAF " RESET " %u\n", values[i]);
        } else {
            leaf = kh_value(tree->index, k);
            for(uint8_t j=0; j<leaf->parents_size; j++) {
                trunk = leaf->parents[j];
                if(DEBUG) printf("set %s to %s %p" RESET "\n", key, trunk->is_leaf ? KGRN : KBLU, trunk->filter);
                filter_set(trunk->filter, key, strlen(key));
            }
            if(DEBUG) printf("set %s to %s %p" RESET "\n", key, leaf->is_leaf ? KGRN : KBLU, leaf->filter);
            filter_set(leaf->filter, key, strlen(key));
        }
    }
    
    return 1;
}

uint8_t ft_fill_tree(struct ft_tree_t* tree, char* filename) {

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    size_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) return 1;

    uint32_t segments[256];
    uint8_t  segments_size = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        
        char *seg, *tab, *end;
        
        end = strchr(line, '\n');     // find end / new line
        if(end == NULL) continue;       
        *end = 0;                     // replace it fith terminator
    
        tab = strchr(line, '\t');     // find tab
        if(tab == NULL) continue;
        *tab = 0;                    // split key/data
        // get last space
        seg  = strrchr(tab+1, ' ');  // find segments
        if(seg == NULL) continue;
        *seg = 0;                    // data to flags/segments
        seg++;
    
        // iterate through segments
        segments_size = 0;
        char *s, *sv;
        s = strtok_r(seg, "/", &sv);
        while (s!=NULL) {
            // incriment segment
            uint32_t seg = (uint32_t) strtoul(s, NULL, 10);
            segments[segments_size++] = seg;
            // next token
            s = strtok_r(NULL, "/", &sv);
        }
        ft_set_key(tree, line, segments, segments_size);
    }

    fclose(fp);
    if (line) free(line);
    return 0;
}

void 
ft_print_tree(struct ft_tree_t* tree) {
    khiter_t k;
    for (k = kh_begin(tree->index); k != kh_end(tree->index); ++k) {
        if (kh_exist(tree->index, k)) {
            struct ft_trunk_t* leaf = kh_value(tree->index, k);
            printf(KGRN "%d" RESET KMAG " %lu" RESET " \t[ " KBLU, leaf->leaf_name, leaf->filter->size);
            for(uint8_t j=0; j<leaf->parents_size; j++) printf("%p ", leaf->parents[j]);
            printf(RESET "]\n");
        }
    }
}

// print trunk and branches
void 
ft_print_trunk(struct ft_trunk_t* trunk) {

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
        ft_print_trunk(trunk->branches[0]);
        ft_print_trunk(trunk->branches[1]);
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
    kh_destroy(i32, tree->index);
    free(tree->buf_names);
    free(tree->buf_integral);
    free(tree);
}
