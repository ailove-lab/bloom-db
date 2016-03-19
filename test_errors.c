#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "colors.h"

int main(int argc, char** argv) {

    FILE* fp1,
        * fp2;
    char* line1 = NULL,
        * line2 = NULL;
    size_t len1 = 0,
           len2 = 0;

    fp1 = fopen(argv[1], "r");
    fp2 = fopen(argv[2], "r");
    if (fp1 == NULL) return 1;
    float average_fpp=0;
    uint64_t j=0;
    char buf[4096] = {0};
    while (
        getline(&line1, &len1, fp1) != -1 &&
        getline(&line2, &len2, fp2) != -1 ) {
        
        j++;

        char* end;
        end = strchr(line2, '\n');     // find end / new line
        if(end == NULL) continue;       
        *end = 0;                     // replace it with terminator
    
        // iterate through segments
        char *s, *sv;
        
        // cut key
        s = strtok_r(line2, " ", &sv);
        int i = 0;
        // printf(KBLU "%s" RESET, line1);
        int p=-1, fp=0;
        char* cur = buf;
        while (s!=NULL) {
            char* sub = strstr(line1, s);
            if(sub) p++; else fp++;
            if(!i)
                printf("%s%s " RESET, sub? KBLU : KRED, s);
            else
                cur += sprintf(cur, "%s" RESET, sub ? KGRN"+" : KRED"-");

            s = strtok_r(NULL, " ", &sv);
            i++;
        }
        float fpp = (float)fp / (float)p;
        printf(KYEL"%2.3f (%2.3f)"RESET" %s", fpp, average_fpp, buf);
        average_fpp = average_fpp*(j-1.0)/j + fpp/j;
        printf("\n");
    }
    printf("average false positive / positive rate: "KRED"%f\n"RESET, average_fpp); 

    fclose(fp1);
    fclose(fp2);
    if (line1) free(line1);
    if (line2) free(line2);

    return 0;
}

