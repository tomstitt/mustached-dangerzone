// Thomas Stitt - CMPSC 473 - Project 3 - 5/1/15
#include "wc_map_reduce.h"


void* map_adder(void* args) {
    adder_args* aa = args;
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

    // init queue
    pthread_mutex_lock_safe(lock);
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
    pthread_mutex_unlock(lock);

    while (cur_addr < start_addr + length) {
        word_length = 0;
        // valid word char
        while ((*cur_addr >= 48 && *cur_addr <= 57) ||
               (*cur_addr >= 65 && *cur_addr <= 90) ||
               (*cur_addr >= 97 && *cur_addr <= 122) ||
               (*cur_addr == 39 || *cur_addr == '-')) {
            word_length++;
        }
        if (word_length > 0) {
            node = malloc(sizeof(wc_node));
            node->word = malloc(word_length + 1);
            memcpy(node->word, start_addr, word_length);
            node->word[word_length] = '\0';
            pthread_mutex_lock_safe(lock);

            while (q->count >= b) {
                pthread_cond_wait()
            }

            if (q->count < b) {
                if (q->count == 0) {
                    q->head = node;
                    q->tail = node;
                }
                else {
                    q->tail->next = node;
                    q->tail = node;
                }
                q->count++;
            }
        }



    }







    return 0;
}

int reduce();

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

    // delete later
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
    pthread_mutex_t** reader_adder_locks = malloc(n * sizeof(pthread_mutex_t));
    pthread_cond_t** reader_adder_queue_cond = malloc(n * sizeof(pthread_cond_t));
    pthread_t** reader_threads = malloc(n * sizeof(pthread_t));
    pthread_t** adder_threads = malloc(n * sizeof(pthread_t));
    queue** reader_adder_buffers = malloc(n * sizeof(queue));
    entry** adder_reducer_buffers = malloc(n * sizeof(entry*));
    reader_args* rargs = malloc(n* sizeof(reader_args));
    adder_args* aargs = malloc(n* sizeof(adder_args));

    // init locks
    int i;
    int step;
    int start_addr;
    int length;
    int replica_len;

    replica_len = file_len / n;

    // init locks and conditionals
    for (i = 0; i < n; i++) {
        assert(pthread_mutex_init(reader_adder_locks[i], NULL) == 0);
        assert(pthread_cond_init(reader_adder_queue_cond[i], NULL) == 0);
    }


    start_addr = 0;
    for (i = 0; i < n; i++) {
        step = 0;
        while (file_buffer[start_addr + length + step] != ' ') {
            step += 1;
        }
        // map-reader arg struct
        rargs[i].start_addr = file_buffer + start_addr;
        rargs[i].length = length + step;
        rargs[i].read_add_buf = reader_adder_buffers[i];
        rargs[i].lock = reader_adder_locks[i];
        rargs[i].cond = reader_adder_queue_cond[i];

        // map-adder arg struct
        aargs[i].read_add_buf = reader_adder_buffers[i];
        aargs[i].add_reduce_buf = adder_reducer_buffers[i];
        aargs[i].lock = reader_adder_locks[i];
        aargs[i].cond = reader_adder_queue_cond[i];

        pthread_create(reader_threads[i], NULL, map_reader, &rargs[i]);
        pthread_create(adder_threads[i], NULL, map_adder, &aargs[i]);

        start_addr += length + step;
    }

    free(file_buffer);
    free(reader_adder_locks);
    free(reader_adder_queue_cond);
    free(reader_adder_buffers);
    free(adder_reducer_buffers);
    free(reader_threads);
    free(adder_threads);
    free(rargs);
    free(aargs);

    return 0;
}


void pthread_mutex_lock_safe(pthread_mutex_t *mutex) {
    assert(pthread_mutex_lock(mutex) == 0);
}
