#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1_function(void* arg) {
    printf("Thread 1: Locking mutex1\n");
    pthread_mutex_lock(&mutex1);
    sleep(1); // Задержка для создания условий для deadlock
    printf("Thread 1: Waiting for mutex2\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread 1: Locked both mutexes\n");
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    return NULL;
}

void* thread2_function(void* arg) {
    printf("Thread 2: Locking mutex2\n");
    pthread_mutex_lock(&mutex2);
    sleep(1); // Задержка для создания условий для deadlock
    printf("Thread 2: Waiting for mutex1\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread 2: Locked both mutexes\n");
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    if (pthread_create(&thread1, NULL, thread1_function, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread2, NULL, thread2_function, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(thread1, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(thread2, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    printf("Both threads finished\n");

    return 0;
}
