#include "common.h"

struct mapNode {char* key; int value; struct mapNode* next;};

// Maps text to void pointers
struct map {struct mapNode** lists;};

struct map* map_create();
void map_put(struct map*, char*, int);
int map_get(struct map*, const char*);