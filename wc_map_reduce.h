// Thomas Stitt - CMPSC 473 - Project 3 - 5/1/15
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "wc_dict.h"

#define NO_MORE_WORDS -1

int b;
char* file_buffer;

typedef struct wc_node {
    char* word;
    int count;
    struct wc_node* next;
} wc_node;

typedef struct queue {
    wc_node* head;
    wc_node* tail;
    int count;
} queue;

typedef struct reader_args {
    char* start_addr;
    int length;
    pthread_mutex_t* lock;
    pthread_cond_t* cond;
    queue* read_add_buf;
} reader_args;

typedef struct adder_args {
    pthread_mutex_t* lock;
    pthread_cond_t* cond;
    queue* read_add_buf;
    entry** add_reduce_buf;
} adder_args;

void* map_adder(void* args);

void* map_reader(void* args);

int reduce();

int main(int argc, char** argv);

int is_word_char(char c);

void pthread_mutex_lock_safe(pthread_mutex_t *mutex);
