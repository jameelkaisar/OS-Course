// Run Using "gcc -pthread 18_producer_consumer_multi_time.c && ./a.out"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX 25
#define OPR 2500
#define PRO 25
#define CON 25

pthread_mutex_t buf_lock;
pthread_mutex_t num_lock;

typedef struct str {
    pthread_mutex_t* buf_lock;
    pthread_mutex_t* num_lock;
    int* buffer;
    int* head;
    int* tail;
    int* num;
    int* time;
} str;

void* producer(pthread_mutex_t* buf_lock, int* buffer, int* head, int* tail, int data) {
    pthread_mutex_lock(buf_lock);
    while ((*tail + 1) % MAX == *head) {
        // printf("Buffer is full. Producer is waiting.\n");
        pthread_mutex_unlock(buf_lock);
        while ((*tail + 1) % MAX == *head);
        pthread_mutex_lock(buf_lock);
    }
    buffer[*tail] = data;
    *tail = (*tail + 1) % MAX;
    pthread_mutex_unlock(buf_lock);

    return NULL;
}

int consume(pthread_mutex_t* buf_lock, int* buffer, int* head, int* tail) {
    pthread_mutex_lock(buf_lock);
    while (*head == *tail) {
        // printf("Buffer is empty. Consumer is waiting.\n");
        pthread_mutex_unlock(buf_lock);
        while (*head == *tail);
        pthread_mutex_lock(buf_lock);
    }
    int data = buffer[*head];
    *head = (*head + 1) % MAX;
    pthread_mutex_unlock(buf_lock);

    return data;
}

void* producer_thread(void* vargp) {
    str* s = (str*)vargp;
    pthread_mutex_t* buf_lock = s->buf_lock;
    pthread_mutex_t* num_lock = s->num_lock;
    int* buffer = s->buffer;
    int* head = s->head;
    int* tail = s->tail;
    int* num = s->num;
    int* t = s->time;
    free(s);
    srand(time(NULL));

    *t = clock();
    for (int i = 0; i < OPR; i++) {
        int data = rand() % 100 + 1;
        pthread_mutex_lock(num_lock);
        // printf("num = %d + %d = %d\n", *num, data, *num + data);
        *num = *num + data;
        pthread_mutex_unlock(num_lock);
        producer(buf_lock, buffer, head, tail, data);
        // usleep(100);
    }
    *t = clock() - *t;

    return NULL;
}

void* consumer_thread(void* vargp) {
    str* s = (str*)vargp;
    pthread_mutex_t* buf_lock = s->buf_lock;
    pthread_mutex_t* num_lock = s->num_lock;
    int* buffer = s->buffer;
    int* head = s->head;
    int* tail = s->tail;
    int* num = s->num;
    int* t = s->time;
    free(s);
    srand(time(NULL));

    *t = clock();
    for (int i = 0; i < OPR; i++) {
        int data = consume(buf_lock, buffer, head, tail);
        pthread_mutex_lock(num_lock);
        // printf("num = %d - %d = %d\n", *num, data, *num - data);
        *num = *num - data;
        pthread_mutex_unlock(num_lock);
        // usleep(100);
    }
    *t = clock() - *t;

    return NULL;
}

int main() {
    int buffer[MAX];
    int head = 0, tail = 0;
    int num = 0;

    pthread_t p[PRO];
    pthread_t c[CON];

    int t_pro[PRO];
    int t_con[CON];

    // Mutex Lock Implementation
    printf("Mutex Lock Implementation (Multiple Producer-Consumer):\n");

    str* s1;

    for (int i = 0; i < PRO; i++) {
        s1 = (str*)malloc(sizeof(str));
        s1->buf_lock = &buf_lock;
        s1->num_lock = &num_lock;
        s1->buffer = buffer;
        s1->head = &head;
        s1->tail = &tail;
        s1->num = &num;
        s1->time = &t_pro[i];
        pthread_create(&p[i], NULL, producer_thread, s1);
    }

    str* s2;

    for (int i = 0; i < CON; i++) {
        s2 = (str*)malloc(sizeof(str));
        s2->buf_lock = &buf_lock;
        s2->num_lock = &num_lock;
        s2->buffer = buffer;
        s2->head = &head;
        s2->tail = &tail;
        s2->num = &num;
        s2->time = &t_con[i];
        pthread_create(&c[i], NULL, consumer_thread, s2);
    }

    for (int i = 0; i < PRO; i++)
        pthread_join(p[i], NULL);

    for (int i = 0; i < CON; i++)
        pthread_join(c[i], NULL);

    printf("Result: %s\n", num == 0 ? "Success" : "Fail");

    // for (int i = 0; i < PRO; i++)
    //     printf("Producer %d: %f\n", i, (double)t_pro[i] / CLOCKS_PER_SEC);
    
    // for (int i = 0; i < CON; i++)
    //     printf("Consumer %d: %f\n", i, (double)t_con[i] / CLOCKS_PER_SEC);
    
    double total_pro = 0;
    for (int i = 0; i < PRO; i++)
        total_pro += (double)t_pro[i] / CLOCKS_PER_SEC;
    printf("Total Producer Time: %f\n", total_pro);

    double total_con = 0;
    for (int i = 0; i < CON; i++)
        total_con += (double)t_con[i] / CLOCKS_PER_SEC;
    printf("Total Consumer Time: %f\n", total_con);

    double total_time = total_pro + total_con;
    printf("Total Time: %f\n", total_time);

    printf("Average Producer Time: %f\n", total_pro / PRO);
    printf("Average Consumer Time: %f\n", total_con / CON);
    printf("Average Time: %f\n", total_time / (PRO + CON));

    return 0;
}
