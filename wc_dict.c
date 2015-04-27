#include "wc_dict.h"

unsigned hash(char *s) {
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++) {
        hashval = *s + 31 * hashval;
    }

    return hashval % HASHSIZE;
}

entry* lookup(entry** dict, char* s)
{
    entry* e;
    for (e = dict[hash(s)]; e != NULL; e = e->next) {
        if (strcmp(s, e->word) == 0) {
          return e;
        }
    }
    return NULL;
}

int insert(entry** dict, char* word) {
    entry* e;
    unsigned hashval;
    // not found, create entry and set to 1
    if ((e = lookup(dict, word)) == NULL) {
        e = (entry*)malloc(sizeof(entry));
        if (e == NULL || (e->word = strdup(word)) == NULL) {
            fprintf(stderr, "Problem adding to dictionary\n");
            return 1;
        }
        e->wc = 1;
        hashval = hash(word);
        e->next = dict[hashval];
        dict[hashval] = e;
    }
    // found, increment
    else {
        e->wc++;
    }

    return 0;
}

