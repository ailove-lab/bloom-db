#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "filter_tree.h"

int main(int argc, char** argv) {

    printf(KYEL "/// BUILD TREE ///" RESET "\n");
    struct ft_tree_t* tree = ft_grow_file("./data/100k.stat");
    ft_fill_tree(tree, "./data/100k");

    char *line = NULL;
    size_t len = 0;
    uint32_t results[256];
    uint8_t  results_size;
    while (getline(&line, &len, stdin) != -1) {
        char* nl = strchr(line, '\n');
        if(nl != NULL) *nl = 0;
        ft_get_key(tree, line, results, &results_size);
        printf("%s ", line);
        for(uint8_t i=0;i<results_size; i++) printf("%u ", results[i]); printf("\n");
    }
    free(line);


    ft_free_tree(tree);

    return 0;
}

