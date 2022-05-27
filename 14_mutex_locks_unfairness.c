// Run Using "gcc -pthread 14_mutex_locks_unfairness.c && ./a.out"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void* simple_increment(void* vargp) {
    int* num = (int*)vargp;

    for (int i = 0; i < 10000; i++) {
        *num = *num + 1;
    }

    return NULL;
}

pthread_mutex_t status;

typedef struct lock {
    pthread_mutex_t* status;
    int* num;
    clock_t* time;
    int* id;
} lock;

void* locked_increment(void* vargp) {
    lock* l = (lock*)vargp;
    int* num = l->num;
    pthread_mutex_t* status = l->status;
    clock_t* time = l->time;
    int* id = l->id;
    free(l);

    *time = clock();
    for (int i = 0; i < 10000; i++) {
        pthread_mutex_lock(status);
        if (*id == 0) usleep(5); // Makes all threads suffer
        *num = *num + 1;
        pthread_mutex_unlock(status);
    }
    *time = clock() - *time;

    return NULL;
}

int main() {
    int num;
    pthread_t tid[5];
    clock_t time[5];
    int id[5];

    // Mutex Locked Increment
    printf("Locked Increment:\n");

    num = 0;

    for (int i = 0; i < 5; i++) {
        lock* l = malloc(sizeof(lock));
        pthread_mutex_init(&status, NULL);
        l->status = &status;
        l->num = &num;
        l->time = &time[i];
        id[i] = i;
        l->id = &id[i];
        pthread_create(&tid[i], NULL, locked_increment, (void*)l);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }
    printf("Number = %d\n", num);
    printf("Result: %s (Diff: %d)\n", num == 50000 ? "PASS" : "FAIL", 50000 - num);

    // print time taken
    for (int i = 0; i < 5; i++) {
        printf("Time taken for thread %d: %f\n", i, (double)time[i] / CLOCKS_PER_SEC);
    }

    return 0;
}
