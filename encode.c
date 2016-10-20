/*

1) Акумулирует статистику по входным данным
2) По статистике строит список блум фильтров
3) Сохраняет фильтры

*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

#include "filter_list.h"

int usage();

int 
main(int argc, char *argv[]) {
    
    if(argc!=2) return usage();
    
    fl_t* fl = fl_encode(argv[1])
    fl_print(fl);
    fl_save(fl, argv[2]);
    fl_destroy(fl);

    return 0;
}

int 
usage() {
    printf("Usage:\n");
    printf("\t./encode input.txt output-bloom.blm\n");
    return 1;
}