#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "filter_tree.h"

int main(int argc, char** argv) {

    printf(KYEL "/// BUILD TREE ///" RESET "\n");
    struct ft_tree_t* tree = ft_grow_file("./data/100k.stat");
    ft_print_tree(tree);

    printf(KYEL "/// SET KEY ///" RESET "\n");
    
    char* key = "abc";

    uint32_t values[] = { 
        40010008u, 40010009u, 40010010u, 40010011u, 
        20010017u, 20010018u, 20010019u, 20010020u, 
    };
    size_t size = sizeof(values)/sizeof(values[0]);
    
    printf(KGRN "SET KEY " RESET "%s ", key);
    for(uint8_t i=0; i<size; i++) printf(KBLU "%u " RESET, values[i]); printf("\n");
    ft_set_key(tree, key, values, size);

    printf(KYEL "/// GET RESULT ///" RESET "\n");

    uint32_t* results = (uint32_t*) calloc(100, sizeof(uint32_t));
    uint8_t results_size = 0;
    printf(KGRN "GET KEY " RESET "%s ", key);
    ft_get_key(tree, key, results, &results_size);

    for(uint8_t i=0; i<results_size; i++) printf(KBLU "%u " RESET, results[i]); printf("\n");
    
    // for(char c1='a'; c1<'z'; c1++)
    //     for(char c2='a'; c2<'z'; c2++)
    //         for(char c3='a'; c3<'z'; c3++)
    //             ft_get_key(tree, (char[]){c1,c2,c3,0}, results, &results_size);
    
    free(results);

    ft_free_tree(tree);

    return 0;
}

