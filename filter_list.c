#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "filter_list.h"

  //////////
 // LIST //
//////////

// Возращает hash-map фильтров

fl_t*
fl_encode(char* filename) {
    fl_t* fl = fl_stat(filename);
    fl_allocate(fl);
    fl_fill(fl, filename);
}


// Создаёт пустые фильтры без алокации данных, заполняет счетчики
fl_t*
fl_stat(char* filename) {

    fl_t* fl = kh_init(key_filter_hm);

    FILE*  fp;
    char*  line = NULL;
    size_t len = 0;
    size_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) return NULL;

    uint16_t i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        fl_parse_line(fl, line);
    }

    // cleanup
    fclose(fp);
    if (line) free(line);
}


// Выделаем реальную память под блум фильтры
void 
fl_allocate(fl_t* fl) {
     // segment counters
     for (khiter_t ki=kh_begin(fl); ki!=kh_end(fl); ++ki) {
         if (kh_exist(fl, ki)) {
            filter_t* f  = kh_value(fl, ki);
            f->size      = size;
            f->error     = error;
            f->bit_size  = (uint64_t) ceil(-(float)size * log(error) / LN4);
            f->byte_size = (uint64_t) ceil((float)f->bit_size / 8.0);
            f->hash_num  = (uint8_t ) ((float)f->bit_size/(float)size * LN2);
            f->entries   = 0;
            f->data      = calloc(f->byte_size, sizeof(uint8_t));
         }
     }
}

// Заполянем филтер данными
void 
fl_fill(fl_t* fl, 
        char* filename) {

    
    
}

// Сохраняем фильтр   
void 
fl_save(fl_t* fl, 
        char* output) {

}


// Загрузка фильтра
fl_t* 
fl_load(char* filename) {

}


// Очистка
void
fl_destroy(fl_t* fl) {
    // segment counters
    for (khiter_t ki=kh_begin(fl); ki!=kh_end(fl); ++ki) {
        if (kh_exist(fl, ki)) {
            filter_t* f  = kh_value(fl, ki);
            // Очищаем фильтры
            free(f->data);
            // Очищаем структуры фильтров
            free(f);
            // Очищаем ключи
            free((char*) kh_key(fl, ki));
        }
    }
    // Удаляем список
    kh_destroy(key_filter_hm, fl);
}

// Парсер строки в следующей структуре:
// base64 key [\t] spaced flags [\x20] eeeslashed segments [\n\0]
void 
fl_parse_line(fl_t* fl, char *line) {
    
    char *seg, *tab, *end;
    
    end = strchr(line, '\n');     // find end / new line
    if(end == NULL) return;       
    *end = 0;                     // replace it fith terminator

    tab = strchr(line, '\t');     // find tab
    if(tab == NULL) return;
    *tab = 0;                     // split key/data
    
    // get last space
    seg  = strrchr(tab+1, ' ');   // find segments
    if(seg == NULL) return;
    *seg = 0;                     // data to flags/segments
    seg++;

    // iterate through segments
    char *s, *sv;
    s = strtok_r(seg, "/", &sv);
    while (s!=NULL) {
        int ret;
        // индекс сегмента
        khiter_t ki = kh_get(key_filter_hm, fl, s);
        if(ki == kh_end(fl)) { 
            // новый key id для ключа
            ki = kh_put(key_filter_hm, fl, strdup(s), &ret);
            //создаем новый фильтр
            kh_value(fl, ki) = calloc(1, sizeof(filter_t));
            kh_value(fl, ki)->size = 1;
        } else {
            kh_value(fl, ki)->size += 1;
        }

        // next token
        s = strtok_r(NULL, "/", &sv);
    }
}


// Print statistic
// void 
// _stat_print() {
//     // segment counters
//     for (khiter_t ki=kh_begin(fl); ki!=kh_end(fl); ++ki) {
//         if (kh_exist(fl, ki)) {
//             char *key = (char*) kh_key(fl, ki);
//             long cnt = kh_value(fl, ki);
//             printf("%s %lu\n", key, cnt);
//         }
//     }
// }

// free keys created by strdup()
// void 
// _destroy_stat() {
//     // segment counters
//     for (khiter_t ki=kh_begin(fl); ki!=kh_end(fl); ++ki) {
//         if (kh_exist(fl, ki)) {
//             free((char*) kh_key(fl, ki));
//         }
//     }
//     // destroy structure
//     kh_destroy(key_filter_hm, fl);
// }
