// Thomas Stitt - CMPSC 473 - Project 3 - 4/5/15

// need a #. this will fail
include "wc_dict.h"

unsigned hash(char *s) {
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++) {
        hashval = *s + 31 * hashval;
    }

    return hashval % DICTSIZE;
}

// iterate over the list at the hash of s and then strcmp 
// and attempt to find s
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

int insert(entry** dict, char* word, int count) {
    entry* e;
    unsigned hashval;
    // not found, create entry and set to count
    if ((e = lookup(dict, word)) == NULL) {
        e = (entry*)malloc(sizeof(entry));
        // dictionary error
        if (e == NULL || (e->word = strdup(word)) == NULL) {
            fprintf(stderr, "Problem adding to dictionary\n");
            return 1;
        }
        e->wc = count;
        hashval = hash(word);
        e->next = dict[hashval];
        dict[hashval] = e;
    }
    // found, increment
    else {
        e->wc += count;
    }

    return 0;
}

// Combine dict2 into dict1
int combine(entry** dict1, entry** dict2) {
    int e;
    entry* p;
    for (e = 0; e < DICTSIZE; e++) {
        for (p = dict2[e]; p != NULL; p = p->next) {
            insert(dict1, p->word, p->wc);
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
