#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICTSIZE 283

typedef struct entry {
    struct entry* next;
    char* word;
    int wc;
} entry;

unsigned hash(char* word);

entry* lookup(entry** dict, char* s);

int insert(entry** dict, char* word);

int combine(entry** dict1, entry** dict2);

void dictionary_print(entry** dict);

void to_lower(char *s);
