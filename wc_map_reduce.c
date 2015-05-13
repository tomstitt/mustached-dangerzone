// Thomas Stitt - CMPSC 473- Project 3 - 4/5/15

#include "wc_map_reduce.h"

void* map_adder(void* args) {
    adder_args* aa = args;
    pthread_mutex_t* lock = aa->lock;
    pthread_cond_t* cond = aa->cond;
    queue* q = aa->read_add_buf;
    entry** dict = aa->add_reduce_buf;
    wc_node* node;
    int count_val;

    do {
        // read to add to the dictionary, let's
        // get the lock
        pthread_mutex_lock_safe(lock);
        // if the queue is empty
        while (q->count < 1) {
            pthread_cond_wait(cond, lock);
        }
        node = q->head;
        // there would be no next
        if (q->count != 0) {
            q->head = node->next;
        }
        // tell map-reader we removed from the que
        // if it was previously full
        q->count--;
        if (q->count == b-1) {
            pthread_cond_signal(cond);
        }
        pthread_mutex_unlock(lock);
        count_val = node->count;

        if (count_val != -1) {
            insert(dict, node->word, 1);
        }
        free(node->word);
        free(node);
    } while (count_val != -1);

    return 0;
}

void* map_reader(void* args) {
    reader_args* ra = args;
    char* start_addr = ra->start_addr;
    char* cur_addr = start_addr;
    int length = ra->length;
    int word_length;
    pthread_mutex_t* lock = ra->lock;
    pthread_cond_t* cond = ra->cond;
    queue* q = ra->read_add_buf;
    wc_node* node;

    while (cur_addr < start_addr + length) {
        // find a valid word character
        while (cur_addr < start_addr + length && !is_word_char(*cur_addr)) {
            cur_addr++;
        }

        // valid word char and not over our length
        word_length = 0;
        while (cur_addr + word_length < start_addr + length &&
               is_word_char(*(cur_addr + word_length))) {
            word_length++;
        }

        // if we actually have a word
        if (word_length > 0) {
            // create a word node
            node = malloc(sizeof(wc_node));
            node->word = malloc(word_length + 1);
            memcpy(node->word, cur_addr, word_length);
            node->word[word_length] = '\0';
            node->next = NULL;
            node->count = 1;

            // get the lock
            pthread_mutex_lock_safe(lock);
            // if the queue full?
            while (q->count >= b) {
                pthread_cond_wait(cond, lock);
            }
            // is the queue empty?
            if (q->count == 0) {
                q->head = node;
                q->tail = node;
            }
            // just add to the queue
            else {
                q->tail->next = node;
                q->tail = node;
            }
            q->count++;
            if (q->count == 1) {
                pthread_cond_signal(cond);
            }
            //printf("added %s\n", node->word);
            pthread_mutex_unlock(lock);
        }
        cur_addr += word_length;
    }

    // send one last node so the consumer knows to exit
    // it has a count of NO_MORE_WORDS
    node = malloc(sizeof(wc_node));
    node->word = NULL;
    node->count = NO_MORE_WORDS;
    node->next = NULL;

    // still need to talk out the locks and do all the checks
    pthread_mutex_lock_safe(lock);
    // if the queue full?
    while (q->count >= b) {
        pthread_cond_wait(cond, lock);
    }
    // is the queue empty?
    if (q->count == 0) {
        q->head = node;
        q->tail = node;
    }
    // just add to the queue
    else {
        q->tail->next = node;
        q->tail = node;
    }
    q->count++;
    if (q->count == 1) {
        pthread_cond_signal(cond);
    }
    pthread_mutex_unlock(lock);

    return 0;
}

int reduce(entry** dicts, int n) {
    // we use dicts[0] as the combine dictionary
    int i;
    for (i = 1; i < n; i++) {
        combine(dicts, dicts + i*DICTSIZE);
    }

    return 0;
}

int main(int argc, char** argv) {
    FILE* f;
    int file_len;
    int n;

    // Need input file, n = replica count, b = shared buffer size
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [input filename] [replica count > 0] [shared buffer size >= 1]\n",
                argv[0]);
        return 1;
    }

    if ((n = atoi(argv[2])) < 1) {
        fprintf(stderr, "n must be strictly greater than 0\n");
        return 1;
    }

    if ((b = atoi(argv[3])) < 1) {
        fprintf(stderr, "b must be strictly greater than 0\n");
        return 1;
    }

    printf ("n is %d, b is %d\n", n, atoi(argv[3]));

    f = fopen(argv[1], "r");
	if (f == NULL) {
        fprintf(stderr, "Problem opening %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    // file size:
    fseek(f, 0, SEEK_END);
    file_len = ftell(f);
    rewind(f);

    // create the memory buffer of the file. less I/O saves lots of time
    if ((file_buffer = malloc(file_len /*sizeOf(char)*/)) == NULL) {
        fprintf(stderr, "Problem with memory allocation\n");
        return 1;
    }

    // read the file into memoery
    if (fread(file_buffer, 1, file_len, f) != file_len) {
        fprintf(stderr, "Problem reading %s: %s\n", argv[1], strerror(errno));
    }

    // close the file
    fclose(f);

    printf("File size is %d\n", file_len);

    // done with opening and jawn so on to the next
    // init buffers and locks and let's go to work
    entry** ar_ds = calloc(n * DICTSIZE, sizeof(entry*));
    queue* ra_queues = calloc(n, sizeof(queue));
    pthread_t rthreads[n];
    pthread_t athreads[n];
    reader_args rargs[n];
    adder_args aargs[n];
    pthread_mutex_t locks[n];
    pthread_cond_t conds[n];

    int i;
    int step;
    int start_addr;
    int replica_len;

    replica_len = file_len / n;

    for (i = 0; i < n; i++) {
        assert(pthread_mutex_init(&locks[i], NULL) == 0);
        assert(pthread_cond_init(&conds[i], NULL) == 0);
    }

    start_addr = 0;
    for (i = 0; i < n; i++) {
        step = 0;
        while (start_addr + replica_len + step < file_len &&
               file_buffer[start_addr + replica_len + step] != ' ') {
            step += 1;
        }
        // map-reader arg struct
        rargs[i].start_addr = file_buffer + start_addr;
        if (i == n-1) {
            rargs[i].length = file_len - start_addr;
        }
        else {
            rargs[i].length = replica_len + step;
        }
        rargs[i].read_add_buf = ra_queues + i;
        rargs[i].lock = &locks[i];
        rargs[i].cond = &conds[i];

        // map-adder arg struct
        aargs[i].read_add_buf = ra_queues + i;
        aargs[i].add_reduce_buf = ar_ds + i*DICTSIZE;
        aargs[i].lock = &locks[i];
        aargs[i].cond = &conds[i];

        // make sure the threads actually spawn
        if (pthread_create(&rthreads[i], NULL, map_reader, &rargs[i]) != 0) {
            fprintf(stderr, "Problem creating reader thread %d, exiting\n", i+1);
            exit(1);
        }
        if (pthread_create(&athreads[i], NULL, map_adder, &aargs[i]) != 0) {
            fprintf(stderr, "Problem creating adder thread %d, exiting\n", i+1);
            exit(1);
        }

        start_addr += replica_len + step;
    }

    // join and destroy the locks and conditionals
    for (i = 0; i < n; i++) {
        pthread_join(rthreads[i], NULL);
        pthread_join(athreads[i], NULL);
    }

    for (i = 0; i < n; i++) {
        assert(pthread_mutex_destroy(&locks[i]) == 0);
        assert(pthread_cond_destroy(&conds[i]) == 0);
    }

    // reduce and print
    reduce(ar_ds, n);
    dictionary_print(ar_ds);

    // cleanup
    free(ar_ds);
    free(ra_queues);
    free(file_buffer);

    return 0;
}


int is_word_char(char c) {
    return (c >= 48 && c <= 57) ||
           (c >= 65 && c <= 90) ||
           (c >= 97 && c <= 122) ||
           (c == 39 || c == '-');
}

void pthread_mutex_lock_safe(pthread_mutex_t *mutex) {
    assert(pthread_mutex_lock(mutex) == 0);
}
