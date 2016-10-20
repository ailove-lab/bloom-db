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
fl_encode(char* filename, float error) {
    fl_t* fl = fl_stat(filename);
    fl_allocate(fl, error);
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
        fl_stat_line(fl, line);
    }

    // cleanup
    fclose(fp);
    if (line) free(line);
}


// Выделаем реальную память под блум фильтры
void 
fl_allocate(fl_t* fl, float error) {
     // segment counters
     for (khiter_t ki=kh_begin(fl); ki!=kh_end(fl); ++ki) {
         if (kh_exist(fl, ki)) {
            filter_t* f  = kh_value(fl, ki);
            // f->size      = size; // calculated in stat
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


// размечаем строку нулями
// base64 key [\t] spaced flags [\x20] slashed segments [\n\0]
//              0                   0                       0
// возращаем начало списка сегментов
char*
fl_tokenize_line(char* line) {

    char *seg, *tab, *end;
    
    end = strchr(line, '\n');     // find end / new line
    if(end == NULL) return NULL;       
    *end = 0;                     // replace it fith terminator

    tab = strchr(line, '\t');     // find tab
    if(tab == NULL) return NULL;
    *tab = 0;                     // split key/data
    
    // get last space
    seg = strrchr(tab+1, ' ');   // find segments
    if(seg == NULL) return NULL;
    *seg = 0;                     // data to flags/segments
    seg++;

    return seg; 
}

// Сбор статистики по сегментам
void 
fl_stat_line(fl_t* fl, 
             char* line) {

    char* seg =fl_tokenize_line(line)
    
    if(seg == NULL) return;

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


// Индексация сегментов
void 
fl_parse_line(fl_t* fl, 
              char* line) {

    char* seg =fl_tokenize_line(line)
    
    if(seg == NULL) return;

    // iterate through segments
    char *s, *sv;
    s = strtok_r(seg, "/", &sv);
    
    // calculate hashes 
    // TODO get real number of hashes for first filter
    // 0.001% - 17; 0.0001 - 20; 0.00001 - 24
    uint32_t hashes[32];
    for(uint32_t i=0; i<32; ++i) murmur3_hash32(line, strlen(line), i, &hashes[i]);

    while (s!=NULL) {
        int ret;
        // индекс сегмента
        khiter_t ki = kh_get(key_filter_hm, fl, s);
        if(ki != kh_end(fl)) { 
            filter_t* f = kh_value(fl, ki);
            filter_set(f, line, hashes)
        }

        // next token
        s = strtok_r(NULL, "/", &sv);
    }
}


// установить биты фильтра
static void 
filter_set(filter_t* f, 
           char* key, 
           uint32_t* hashes) {

    if(f == NULL) return;

    for (uint32_t i = 0; i < f->hash_num; i++) {
        uint32_t h = hashes[i];
        h %= f->bit_size;
        bit_set(f->data, h);
    }
    f->entries++;

}

// проверить биты фильтра
static uint8_t 
filter_get(filter_t* f, 
           const char* key, 
           size_t len) {

    uint32_t i;
    uint32_t h;

    for (i = 0; i < f->hash_num; i++) {
        murmur3_hash32(key, len, i, &h);
        h %= f->bit_size;
        if (!bit_get(f->data, h))
            return 0;
    }
    return 1;
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
