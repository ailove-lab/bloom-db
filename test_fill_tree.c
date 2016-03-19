#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "filter_tree.h"
#include "timer.h"

#ifndef TEST_FILE
#define TEST_FILE "100k"
#endif

int main(int argc, char** argv) {

    fprintf(stderr, KYEL "/// BUILD TREE ///" RESET "\n");
    struct ft_tree_t* tree = ft_grow_file("./data/" TEST_FILE ".stat");
    fprintf(stderr, KYEL "/// FILL TREE ///" RESET "\n");
    ft_fill_tree(tree, "./data/" TEST_FILE);

    fprintf(stderr, KYEL "/// RESTORE KEYS ///" RESET "\n");

    char *line = NULL;
    size_t len = 0;
    uint32_t results[256];
    uint8_t  results_size;
    uint64_t line_counter = 0;

    while (getline(&line, &len, stdin) != -1) {
        line_counter++;
        if((line_counter) % 10000   == 0) fprintf(stderr, ".");
        if((line_counter) % 1000000 == 0) {
            fprintf(stderr, "\n");
            timer_stop();
            fprintf(stderr, "speed:" KGRN "%llu" RESET " krps\n", 1000000/last_timer);
            timer_start();
        }  
        char* nl = strchr(line, '\n');
        if(nl != NULL) *nl = 0;
        ft_get_key(tree, line, results, &results_size);
        printf("%s ", line);
        for(uint8_t i=0;i<results_size; i++) printf("%u ", results[i]); printf("\n");
    }
    fprintf(stderr, "\n");
    free(line);


    ft_free_tree(tree);

    return 0;
}

