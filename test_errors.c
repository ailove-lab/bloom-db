#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

int main(int argc, char** argv) {

    FILE* fp1,
        * fp2;
    char* line1 = NULL,
        * line2 = NULL;
    size_t len1 = 0,
           len2 = 0;
    size_t read1,
           read2;

    fp1 = fopen(argv[1], "r");
    fp2 = fopen(argv[2], "r");
    if (fp1 == NULL) return 1;

    while (

        getline(&line1, &len1, fp1) != -1 &&
        getline(&line2, &len2, fp2) != -1 ) {
        
        char* end;
        end = strchr(line2, '\n');     // find end / new line
        if(end == NULL) continue;       
        *end = 0;                     // replace it with terminator
    
        // iterate through segments
        char *s, *sv;
        
        // cut key
        s = strtok_r(line2, " ", &sv);
        int i = 0;
        while (s!=NULL) {
            char* sub = strstr(line1, s);
            printf("%s%s" RESET, sub == NULL ? KRED : KGRN, i==0 ? s : "#");
            s = strtok_r(NULL, " ", &sv);
            i++;
        }
        printf("\n");
    }

    fclose(fp1);
    fclose(fp2);
    if (line1) free(line1);
    if (line2) free(line2);

    return 0;
}

