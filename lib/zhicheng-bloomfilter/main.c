#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bloomfilter.h"
/* 
Optimal values: fp error, bits/entry, hashes

*/

void printbit(unsigned char);
void printbitvector(unsigned char*, int);

int
main(void)
{
        struct bloomfilter *bloomfilter;
        
        float p = 0.1; // 1%
        int n = 'G'-'A' + 1;
        float m = -n*log(p)/log(2)/log(2);
        float k = m/n*log(2);

        uint b = (uint) ceil(m/8);

        printf("Error   p: %f (%d%%)\n", p, (int)(p*100));
        printf("Entries n: %d\n", n);
        printf("Bits    m: %f (%d)\n", m, b);
        printf("Hashes  k: %f (%d)\n", k, (int)ceil(k));
        printf("\n");

        bloomfilter = malloc(sizeof(struct bloomfilter) + b);
        bloomfilter_init(bloomfilter, b << 3, (uint)ceil(k));

        printbitvector(bloomfilter->bit_vector, b);
        printf("\n");
        for (unsigned char c = 'A'; c <= 'G'; c++) {
                bloomfilter_set(bloomfilter, &c, 1);
                printbitvector(bloomfilter->bit_vector, b);
                printf(" << %c\n", c);
        }

        printf("\n");
        for (unsigned char c = ' '; c <= 'z'; c++) {
                printf("%d", bloomfilter_get(bloomfilter, &c, 1));
        }
        printf("\n");

        for (unsigned char c = ' '; c <= 'z'; c++) {
                printf("%c", c);
        }
        printf("\n");
}

void 
printbit(unsigned char v) {
        for(unsigned char z=128; z>0; z>>=1) printf("%s", v & z ? "1" : "0");
}

void 
printbitvector(unsigned char* v, int l) {
        for(int i=0; i<l; i++) printbit(v[i]);
}
