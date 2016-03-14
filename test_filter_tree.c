#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "filters/filter.h"

void split_list(uint16_t* list, uint32_t len, float factor);
int load_stat(char* filename);

#define SEG_MAX 3000
#define PRINT_TREE 0

// Segment buffers
uint16_t seg_count;
uint16_t seg_index[SEG_MAX];
uint32_t seg_names[SEG_MAX];
uint32_t seg_stat [SEG_MAX];
uint32_t seg_sigma[SEG_MAX];

uint64_t cumulative_entries;

int main(int argc, char** argv) {

    struct filter_t* f = filter_new(1000, 0.5);
    printf("%lu %f\n", f->size, f->error);

    printf("%d\n", filter_get(f, "a", 1));
    filter_set(f, "a", 1);
    printf("%d\n", filter_get(f, "a", 1));
    printf("%d\n", filter_get(f, "b", 1));
    filter_print(f);

    filter_free(f);
    load_stat("./data/sorted.stat");
    cumulative_entries = 0;
    split_list(seg_index, seg_count, 0.5);

    return 0;
}

void split_list(uint16_t* list, uint32_t len, float factor) {
    
    if(factor>1.0 || factor<=0) return;

    static uint8_t tc = 0;
    if(PRINT_TREE) for (int i=0; i<tc; i++) printf("  ");
    tc++;

    uint32_t size[3];
    size[0] = seg_sigma[list[0]];
    size[1] = seg_sigma[list[len-1]+1];
    size[2] = size[1]-size[0];
    
    if(len<=1) {
        cumulative_entries += seg_stat[list[0]];
        if(PRINT_TREE)  printf("<%d>: %u\n", list[0], seg_stat[list[0]]);
        tc--;
        printf("%u %u %d\n", seg_names[list[0]], seg_stat[list[0]], tc);
        return;
    }
    
    cumulative_entries += size[2];
    if(PRINT_TREE) printf("[%d-%d]: %u \n", list[0], list[len-1], size[2]);
    uint32_t s = floor(len * factor);
    if (s<1) s = 1;
    uint32_t l1 = s;
    uint32_t l2 = len - s;

    if(l1>0) split_list(list, l1, factor)     ;
    if(l2>0) split_list(list + l1, l2, factor);
    // else printf("(%d)\n", list[l1]);

    tc--;
}

int load_stat(char *filename) {

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    size_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) return -1;

    uint16_t i = 0;
    seg_sigma[i]=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        char *cnt = strchr(line, ' ');
        if (cnt == NULL) continue;
        *cnt = 0; cnt++;
        seg_index[i] = i;
        seg_names[i] = atol(line);
        seg_stat[i]  = atol(cnt);
        //printf("%s %u %u\n", line, seg_names[i], seg_stat[i]);
        seg_sigma[i+1] = seg_sigma[i]+seg_stat[i];
        i++;
        if(i>=SEG_MAX) break;
    }
    seg_count = i;

    fclose(fp);
    if (line) free(line);
    return 0;
}

