#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int start;
    int end;
    int mod;
} ThreadArgs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int result = 1;

void* compute_factorial(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    int start = thread_args->start;
    int end = thread_args->end;
    int mod = thread_args->mod;
    int local_result = 1;

    for (int i = start; i <= end; i++) {
        local_result = (local_result * i) % mod;
    }

    pthread_mutex_lock(&mutex);
    result = (result * local_result) % mod;
    pthread_mutex_unlock(&mutex);

    free(thread_args);
    return NULL;
}

int main(int argc, char* argv[]) {
    int k = 0;
    int pnum = 1;
    int mod = 1;
    int opt;

    while ((opt = getopt(argc, argv, "k:p:m:")) != -1) {
        switch (opt) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'p':
                pnum = atoi(optarg);
                break;
            case 'm':
                mod = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -k <number> -p <threads> -m <mod>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Usage: %s -k <number> -p <threads> -m <mod>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pthread_t threads[pnum];
    int chunk_size = k / pnum;

    for (int i = 0; i < pnum; i++) {
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args->start = i * chunk_size + 1;
        args->end = (i == pnum - 1) ? k : (i + 1) * chunk_size;
        args->mod = mod;

        if (pthread_create(&threads[i], NULL, compute_factorial, (void*)args) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    printf("%d! mod %d = %d\n", k, mod, result);

    return 0;
}
