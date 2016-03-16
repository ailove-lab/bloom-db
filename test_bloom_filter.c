#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "filter.h"

int main(int argc, char** argv) {

    struct filter_t* f = filter_new(1000, 0.5);
    filter_print(f);
    filter_free(f);

    return 0;
}

