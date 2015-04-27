#include "wc_dict.h"

unsigned hash(char *s) {
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++) {
        hashval = *s + 31 * hashval;
    }

    return hashval % DICTSIZE;
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

// Combine dict2 into dict1
int combine(entry** dict1, entry** dict2) {
    int e;
    entry* p;
    for (e = 0; e < DICTSIZE; e++) {
        for (p = dict2[e]; p != NULL; p = p->next) {
            insert(dict1, p->word);
        }
    }

    return 0;
}

void dictionary_print(entry** dict) {
    int e;
    entry* p;
    for (e = 0; e < DICTSIZE; e++) {
        for (p = dict[e]; p != NULL; p = p->next) {
            printf("%s - %d\n", p->word, p->wc);
        }
    }
}

void to_lower(char *s) {
    for (;*s != '\0'; s++) {
//        if (s k
    }

}
