#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "filter_tree.h"

int main(int argc, char** argv) {

    struct ft_tree_t* tree = ft_grow_file("./data/100k.stat");

    printf("SET KEYS\n");
    uint32_t values[] = { 
        20010017u, 20010018u, 20010019u, 20010020u, 
        20010021u, 20010022u, 20010023u, 20010024u, 
        20010025u, 20010026u, 20010027u, 20010028u, 
        40010008u, 40010009u, 40010010u, 40010011u, 
    };
    ft_set_key(tree, "abc", values, sizeof(values));
    printf("%lu\n", sizeof(values)/sizeof(values[0]));

    printf("/// GET RESULTS ///\n");
    uint32_t* results = (uint32_t*) calloc(100, sizeof(uint32_t));
    uint8_t results_size = 0;
    ft_get_key(tree, "abc", results, &results_size);
    for(uint8_t i=0; i<results_size; i++) printf("%u\n", results[i]);
    
    // for(char c1='a'; c1<'z'; c1++)
    //     for(char c2='a'; c2<'z'; c2++)
    //         for(char c3='a'; c3<'z'; c3++)
    //             ft_get_key(tree, (char[]){c1,c2,c3,0}, results, &results_size);
    
    free(results);

    ft_free_tree(tree);

    return 0;
}

