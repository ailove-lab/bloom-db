#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "filter_tree.h"

void split_list(uint16_t* list, uint32_t len, float factor);
int load_stat(char* filename);

int main(int argc, char** argv) {

    struct ft_tree_t* tree = ft_grow_file("./data/100k.stat");
    ft_print(tree->root);
    ft_free_tree(tree);

    return 0;
}

