#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Line {
    // Valid as 0 means this cache line is not being used
    // Valid as 1 means this cache line is being used
    unsigned short valid;
    unsigned long set_index;
    unsigned long tag;
    unsigned long long block;

    // When was this cache line last hit?
    unsigned int last_hit_time;
    // How many times has this cache line been used (with current contents)?
    unsigned int times_used;
} Line;

typedef struct Set {
    Line * lines;
} Set;

typedef struct Cache {
    unsigned int num_sets;
    unsigned int num_lines_per_set;
    unsigned int block_size;
    unsigned int num_address_bits;

    unsigned int num_set_bits;
    unsigned int num_block_bits;
    unsigned int num_tag_bits;

    unsigned int hit_time;
    unsigned int miss_time;

    char replacement_policy[4];

    Set * sets;
} Cache;

Line * create_lines_for_set(unsigned int num_lines_per_set) {
    Line * new_lines = (Line*) malloc(num_lines_per_set * sizeof(Line));
    for (int i = 0; i < num_lines_per_set; i++) {
        new_lines[i].valid = 0;
        new_lines[i].last_hit_time = 0;
        new_lines[i].times_used = 0;
    }
    return new_lines;
}

Set * create_sets(unsigned int num_sets, unsigned int num_lines_per_set) {
    Set * new_sets = (Set*) malloc(num_sets * sizeof(Set));
    for (int i = 0; i < num_sets; i++) {
        // Create the array of Line structs for each Set
        new_sets[i].lines = create_lines_for_set(num_lines_per_set);
    }
    return new_sets;
}

Cache * create_cache(unsigned int num_sets, unsigned int num_lines_per_set,
                     unsigned int block_size, unsigned int num_address_bits,
                     char replacement_policy[4], unsigned int hit_time, unsigned int miss_time) {
    Cache * new_cache = (Cache*) malloc(sizeof(Cache));
    new_cache->num_sets = num_sets;
    new_cache->num_lines_per_set = num_lines_per_set;
    new_cache->block_size = block_size;
    new_cache->num_address_bits = num_address_bits;

    strcpy(new_cache->replacement_policy, replacement_policy);
    new_cache->hit_time = hit_time;
    new_cache->miss_time = miss_time;
    new_cache->sets = create_sets(num_sets, num_lines_per_set);

    // Compute the number of bits for set, block and tag
    new_cache->num_set_bits = log2(num_sets);
    new_cache->num_block_bits = log2(block_size);
    new_cache->num_tag_bits = num_address_bits - (new_cache->num_set_bits + new_cache->num_block_bits);

    return new_cache;
}

unsigned int read_from_cache(int address_to_read, Cache * cache, unsigned int current_time) {
    unsigned long long address = address_to_read;
    unsigned long long tag = address >> (cache->num_set_bits + cache->num_block_bits);
    unsigned int num_bits_long_long = sizeof(unsigned long long) * 8;
    unsigned long long set_index = (address << (num_bits_long_long - cache->num_address_bits + cache->num_tag_bits)) >> (num_bits_long_long - cache->num_set_bits);
    unsigned long long block_bits = (address << (num_bits_long_long - cache->num_block_bits)) >> (num_bits_long_long - cache->num_block_bits);

    for (int i = 0; i < cache->num_lines_per_set; i++) {
        if(cache->sets[set_index].lines[i].valid == 1 && cache->sets[set_index].lines[i].tag == tag) {
            cache->sets[set_index].lines[i].last_hit_time = current_time;
            cache->sets[set_index].lines[i].times_used++;
            return cache->hit_time;
        }
    }

    int free_line_index = -1;
    for(int i = 0; i < cache->num_lines_per_set; i++) {
        if (cache->sets[set_index].lines[i].valid != 1) {
            cache->sets[set_index].lines[i].tag = tag;
            cache->sets[set_index].lines[i].set_index = set_index;
            cache->sets[set_index].lines[i].valid= 1;
            cache->sets[set_index].lines[i].block = block_bits;
            cache->sets[set_index].lines[i].last_hit_time = current_time;
            cache->sets[set_index].lines[i].times_used = 1;
            return cache->miss_time;
        }
    }

    // Set is completely full, need to evict a line
    if (strcmp(cache->replacement_policy, "LRU") == 0) {
        int oldest_time = cache->sets[set_index].lines[0].last_hit_time;
        unsigned int oldest_time_index = 0;

        for (int i = 0; i < cache->num_lines_per_set; i++) {
            if (cache->sets[set_index].lines[i].last_hit_time < oldest_time) {
                oldest_time = cache->sets[set_index].lines[i].last_hit_time;
                oldest_time_index = i;
            }
        }
        // Overwrite the least recently used cache line in the set
        cache->sets[set_index].lines[oldest_time_index].tag = tag;
        cache->sets[set_index].lines[oldest_time_index].set_index = set_index;
        cache->sets[set_index].lines[oldest_time_index].valid = 1;
        cache->sets[set_index].lines[oldest_time_index].block = block_bits;
        cache->sets[set_index].lines[oldest_time_index].last_hit_time = current_time;
        cache->sets[set_index].lines[oldest_time_index].times_used = 1;
        return cache->miss_time;
    } else if (strcmp(cache->replacement_policy, "LFU") == 0) {
        int least_used = cache->sets[set_index].lines[0].times_used;
        unsigned int least_used_index = 0;

        for (int i = 0; i < cache->num_lines_per_set; i++) {
            if (cache->sets[set_index].lines[i].times_used < least_used) {
                least_used = cache->sets[set_index].lines[i].times_used;
                least_used_index = i;
            }
        }
        // Overwrite the least frequently used cache line in the set
        cache->sets[set_index].lines[least_used_index].tag = tag;
        cache->sets[set_index].lines[least_used_index].set_index = set_index;
        cache->sets[set_index].lines[least_used_index].valid = 1;
        cache->sets[set_index].lines[least_used_index].block = block_bits; // Can ignore
        cache->sets[set_index].lines[least_used_index].last_hit_time = current_time;
        cache->sets[set_index].lines[least_used_index].times_used = 1;
        return cache->miss_time;
    } else {
        printf("INVALID REPLACEMENT POLICY (Please use LRU or LFU)\n\n");
    }
}


int main() {
    unsigned int num_sets;
    unsigned int num_lines_per_set;
    unsigned int block_size;
    unsigned int num_address_bits;
    unsigned int hit_time;
    unsigned int miss_time;
    char replacement_policy[4];

    scanf("%d", &num_sets);
    scanf("%d", &num_lines_per_set);
    scanf("%d", &block_size);
    scanf("%d\n", &num_address_bits);
    scanf("%c%c%c", &replacement_policy[0], &replacement_policy[1], &replacement_policy[2]);
    // Add the null terminator to the replacement policy string
    replacement_policy[3] = '\0';
    scanf("%d", &hit_time);
    scanf("%d", &miss_time);

    /*
   printf("S = %d\n",num_sets);
   printf("E = %d\n",num_lines_per_set);
   printf("B = %d\n",block_size);
   printf("m = %d\n",num_address_bits);
   printf("Replacement Policy : %s\n",replacement_policy);
   printf("Hit Time : %d\n",hit_time);
   printf("Miss Time  : %d\n",miss_time);
   */

    Cache *cache = create_cache(num_sets, num_lines_per_set, block_size, num_address_bits, replacement_policy, hit_time,
                                miss_time);

    unsigned int time_elapsed = 0;
    unsigned int num_hits = 0;
    unsigned int num_misses = 0;

    int address_to_read;

    unsigned int hex_width_of_address = num_address_bits / 4;

    scanf("%x", &address_to_read);
    while (address_to_read != -1) {
        unsigned int cache_read_time = read_from_cache(address_to_read, cache, time_elapsed);
        time_elapsed += cache_read_time;

        // Print leading 0's according to the hex_width of the address
        if (hex_width_of_address == 2) {
            printf("%02x ", address_to_read);
        } else if (hex_width_of_address == 4) {
            printf("%04x ", address_to_read);
        } else if (hex_width_of_address == 8) {
            printf("%08x ", address_to_read);
        } else {
            printf("%08x ", address_to_read);
        }

        if (cache_read_time == hit_time) {
            printf("H\n");
            num_hits++;
        } else if (cache_read_time == miss_time) {
            printf("M\n");
            num_misses++;
        }
        scanf("%x", &address_to_read);
    }
    printf("%d %d\n", (int)((double) num_misses / (num_hits+num_misses) * 100), num_hits * cache->hit_time +  num_misses * (cache->hit_time + cache->miss_time));
    return 0;
}
