#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 283

typedef struct entry {
    struct entry* next;
    char* word;
    int wc;
} entry;

unsigned hash(char* word);

entry* lookup(entry** dict, char* s);

int insert(entry** dict, char* word);
