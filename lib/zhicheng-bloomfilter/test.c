#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bloomfilter.h"

void printbit(unsigned char);
void printbitvector(unsigned char*, int);
void rand_string(char* str, size_t size);

int
main(void) {

        struct bloomfilter *bloomfilter;
        char key[24];

        float p = 0.1; // 1%
        int step = 100000;
        for(int n=step; n<10*step; n+=step) {
                
                float m = -n*log(p)/log(2)/log(2);
                float k = m/n*log(2);

                uint b = (uint) ceil(m/8);

                bloomfilter = malloc(sizeof(struct bloomfilter) + b);
                bloomfilter_init(bloomfilter, b << 3, (uint)ceil(k));

                #pragma omp parallel for
                for (int i=0; i<n; i++) {
                        rand_string(key, 24);
                        bloomfilter_set(bloomfilter, key, 21);
                }

                // collisions
                int c = 0;
                #pragma omp parallel for
                for (int i=0; i<n; i++) {
                        rand_string(key, 24);
                        if(bloomfilter_get(bloomfilter, key, 21)) c++;
                }
                printf("[p: %.2f%%\tm: %.2f\tk: %.2f\tc: %d\tn: %d]\terr: %.2f%%\n", p*100, m, k, c, n, (float)c/(float)n*100);
                free(bloomfilter);

        }
}

void 
printbit(unsigned char v) {
        for(unsigned char z=128; z>0; z>>=1) printf("%s", v & z ? "1" : "0");
}

void 
printbitvector(unsigned char* v, int l) {
        for(int i=0; i<l; i++) printbit(v[i]);
}

void 
rand_string(char* str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890+/";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
}
