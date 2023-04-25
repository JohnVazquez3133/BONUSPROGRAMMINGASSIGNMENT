#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CACHE_SIZE 32
#define BLOCK_SIZE 4


typedef struct {
    int valid;
    int tag;
    int counter;
} cache_line;


typedef struct {
    cache_line *lines;
    int num_lines;
    int policy; // 0 for LRU, 1 for random
} cache_set;


typedef struct {
    cache_set *sets;
    int num_sets;
    int num_ways;
} cache;


void init_cache(cache *c, int num_sets, int num_ways, int policy) {
    c->num_sets = num_sets;
    c->num_ways = num_ways;
    c->sets = (cache_set*) malloc(num_sets * sizeof(cache_set));
    for (int i = 0; i < num_sets; i++) {
        c->sets[i].num_lines = num_ways;
        c->sets[i].policy = policy;
        c->sets[i].lines = (cache_line*) malloc(num_ways * sizeof(cache_line));
        for (int j = 0; j < num_ways; j++) {
            c->sets[i].lines[j].valid = 0;
            c->sets[i].lines[j].tag = -1;
            c->sets[i].lines[j].counter = 0;
        }
    }
}


void access_cache(cache *c, int address) {
    int set_index = (address / BLOCK_SIZE) % c->num_sets;
    int tag = address / (BLOCK_SIZE * c->num_sets);
    cache_set *set = &(c->sets[set_index]);
    int hit = 0;
    int lru_index = 0;
    int random_index = 0;
    int lru_counter = set->lines[0].counter;
    int random_offset = rand() % set->num_lines;


    for (int i = 0; i < set->num_lines; i++) {
        if (set->lines[i].valid && set->lines[i].tag == tag) {
            hit = 1;
            set->lines[i].counter = 0;
        } else {
            set->lines[i].counter++;
        }
        if (set->policy == 0 && set->lines[i].counter > lru_counter) {
            lru_index = i;
            lru_counter = set->lines[i].counter;
        }
    }


    if (!hit) {
        if (set->policy == 0) {
            set->lines[lru_index].valid = 1;
            set->lines[lru_index].tag = tag;
            set->lines[lru_index].counter = 0;
        } else if (set->policy == 1) {
            set->lines[random_offset].valid = 1;
            set->lines[random_offset].tag = tag;
            set->lines[random_offset].counter = 0;
        }
    }
}


void print_cache(cache *c) {
    for (int i = 0; i < c->num_sets; i++) {
        printf("Set %d: ", i);
        for (int j = 0; j < c->num_ways; j++) {
            printf("[V:%d T:%d C:%d] ", c->sets[i].lines[j].valid, c->sets[i].lines[j].tag, c->sets[i].lines[j].counter);
                printf("\n");
        }
    }
}
int main() {
    FILE *fp;
    char line[100];
    fp = fopen("traces.txt", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    // Direct-mapped cache with LRU replacement policy
    cache direct_mapped_lru;
    init_cache(&direct_mapped_lru, CACHE_SIZE / BLOCK_SIZE, 1, 0);
    printf("Direct-mapped cache with LRU replacement policy:\n");
    while (fgets(line, sizeof(line), fp) != NULL) {
        unsigned int address = (unsigned int)strtol(line, NULL, 16);
        access_cache(&direct_mapped_lru, address);
    }
    print_cache(&direct_mapped_lru);

    // Direct-mapped cache with random replacement policy
    cache direct_mapped_random;
    init_cache(&direct_mapped_random, CACHE_SIZE / BLOCK_SIZE, 1, 1);
    printf("Direct-mapped cache with random replacement policy:\n");
    rewind(fp);
    while (fgets(line, sizeof(line), fp) != NULL) {
        unsigned int address = (unsigned int)strtol(line, NULL, 16);
        access_cache(&direct_mapped_random, address);
    }
    print_cache(&direct_mapped_random);

    // 2-way set-associative cache with LRU replacement policy
    cache two_way_lru;
    init_cache(&two_way_lru, CACHE_SIZE / (BLOCK_SIZE * 2), 2, 0);
    printf("4-way set-associative cache with LRU replacement policy:\n");
    rewind(fp);
    while (fgets(line, sizeof(line), fp) != NULL) {
        unsigned int address = (unsigned int)strtol(line, NULL, 16);
        access_cache(&two_way_lru, address);
    }
    print_cache(&two_way_lru);

    // 2-way set-associative cache with random replacement policy
    cache two_way_random;
    init_cache(&two_way_random, CACHE_SIZE / (BLOCK_SIZE * 2), 2, 1);
    printf("4-way set-associative cache with random replacement policy:\n");
    rewind(fp);
    while (fgets(line, sizeof(line), fp) != NULL) {
        unsigned int address = (unsigned int)strtol(line, NULL, 16);
        access_cache(&two_way_random, address);
    }
    print_cache(&two_way_random);

    // 2-way set-associative cache with LRU replacement policy
    cache four_way_lru;
    init_cache(&four_way_lru, CACHE_SIZE / (BLOCK_SIZE * 4), 4, 0);
    printf("2-way set-associative cache with LRU replacement policy:\n");
    rewind(fp);
    while (fgets(line, sizeof(line), fp) != NULL) {
        unsigned int address = (unsigned int)strtol(line, NULL, 16);
        access_cache(&four_way_lru, address);
    }
    print_cache(&four_way_lru);




     // 4-way set-associative cache with LRU replacement policy
    cache four_way_lru;
    init_cache(&four_way_lru, CACHE_SIZE / (BLOCK_SIZE * 4), 4, 0);
    printf("2-way set-associative cache with LRU replacement policy:\n");
    rewind(fp);
    while (fgets(line, sizeof(line), fp) != NULL) {
        unsigned int address = (unsigned int)strtol(line, NULL, 16);
        access_cache(&four_way_lru, address);
    }
    print_cache(&four_way_lru);

    


    fclose(fp);
    exit(EXIT_SUCCESS);
}