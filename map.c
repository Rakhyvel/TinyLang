/*
    Maps are used to associate string names with pieces of data. They can give 
    almost instantaneous lookup capabilities. 
    
    Author: Joseph Shimel
    Date: 11/4/20 
*/

#include "include.h"

/*
    Function that converts every string to an almost unique number.
    
    Shamelessly copied from somewhere on StackOverflow */
int hash(const char* str, struct map* map) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) hash = hash * 33 + c;

    return abs(hash) % 10;
}

/*
    Creates a map pointer */
struct map* map_create() {
    struct map* map = (struct map*)malloc(sizeof(map));
    map->lists = (struct mapNode**)calloc(10, sizeof(struct mapNode*));
    return map;
}

/*
    Associates a string value with a pointer value in the map */
void map_put(struct map* map, char* key, int value) {
    unsigned int hashcode = hash(key, map);
    for (struct mapNode* bucket = map->lists[hash(key, map)]; bucket != NULL; bucket = bucket->next)
        if (!strcmp(bucket->key, key)) {
            bucket->value = value;
            return;
        }
    struct mapNode* node = (struct mapNode*) malloc(sizeof(struct mapNode));
    node->key = key;
    node->value = value;
    node->next = map->lists[hashcode];
    map->lists[hashcode] = node;
}

/*
    Returns a the pointer associated with a given string key. Returns NULL if 
    key is not in map. */
int map_get(struct map* map, const char* key) {
    for (struct mapNode* bucket = map->lists[hash(key, map)]; bucket != NULL; bucket = bucket->next)
        if (!strcmp(bucket->key, key))
            return bucket->value;
    return 0;
}

bool map_contains(struct map* map, const char* key) {
    for (struct mapNode* bucket = map->lists[hash(key, map)]; bucket != NULL; bucket = bucket->next)
        if (!strcmp(bucket->key, key))
            return true;
    return false;
}