#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define TOTAL 2
#define EMPTY 0
#define WOMEN 1
#define MEN 2

#define N 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t womenSemaphore, menSemaphore;

int womenProduced = 0, womenConsumed = 0;
int menProduced = 0, menConsumed = 0;
int bathroom = EMPTY;

void *womanEnters(void *);
void *manEnters(void *);
void *womanLeaves(void *);
void *manLeaves(void *);

int main(int argc, const char *argv[]) {
    srand((int) time(NULL));  

    pthread_t *threads = (pthread_t *) malloc (sizeof(pthread_t) * 4);
    pthread_t *aux = threads;
    
    sem_init(&womenSemaphore, 0, 0);
    sem_init(&menSemaphore, 0, 0);
    
    pthread_create(aux, NULL, &womanEnters, NULL);
    pthread_create(++aux, NULL, &womanLeaves, NULL);
    pthread_create(++aux, NULL, &manEnters, NULL);
    pthread_create(++aux, NULL, &manLeaves, NULL);
    
    for (aux = threads; aux < threads + 4; aux++)
        pthread_join(*aux, NULL);
    
    free(threads);

    sem_destroy(&womenSemaphore);
    sem_destroy(&menSemaphore);

    pthread_exit(NULL);
}

void *womanEnters(void *arg) {
    int womenInQueue = 0;
    int i = 0;
    while (i < N) {
        usleep(rand() % 300);
        printf("(W) WOMAN has arrived to the queue (%d waiting)\n", ++womenInQueue);
        i++;
        pthread_mutex_lock(&mutex);
        if (bathroom == EMPTY) {
            bathroom = WOMEN;
        }
        pthread_mutex_unlock(&mutex);
        if (bathroom != WOMEN) {
            printf("(W) WOMAN cannot enter the bathroom\n");
        }
        sem_post(&womenSemaphore);
        printf("(W) --> WOMAN has entered the bathroom (%d waiting)\n", --womenInQueue);
    }
    pthread_exit(NULL);
}

void *manEnters(void *arg) {
    int menInQueue = 0;
    int i = 0;
    while (i < N) {
        usleep(rand() % 300);
        printf("(M) MAN has arrived to the queue (%d waiting)\n", ++menInQueue);
        i++;
        pthread_mutex_lock(&mutex);
        if (bathroom == EMPTY) {
            bathroom = MEN;
        }
        pthread_mutex_unlock(&mutex);
        if (bathroom != MEN) {
            printf("(M) MAN cannot enter the bathroom\n");
        }
        sem_post(&menSemaphore);
        printf("(M) --> MAN has entered the bathroom (%d waiting)\n", --menInQueue);
    }
    pthread_exit(NULL);
}

void *womanLeaves(void *arg) {
    int womenInBathroom;
    int i = 0;
    while (i < N) {
        sem_wait(&womenSemaphore);
        printf("(W) <-- WOMAN has left the bathroom (%d inside)\n", womenInBathroom);
        sem_getvalue(&womenSemaphore, &womenInBathroom);
        i++;
        if (!womenInBathroom) {
            usleep(rand() % 300);
            pthread_mutex_lock(&mutex);
            bathroom = EMPTY;
            printf("Bathroom is empty\n");
            pthread_mutex_unlock(&mutex);
        }

    }
    pthread_exit(NULL);
}

void *manLeaves(void *arg) {
    int menInBathroom;
    int i = 0;
    while (i < N) {
        sem_wait(&menSemaphore);
        sem_getvalue(&menSemaphore, &menInBathroom);
        printf("(M) <-- MAN has left the bathroom (%d inside)\n", menInBathroom);
        i++;
        if (!menInBathroom) {
            usleep(rand() % 300);
            pthread_mutex_lock(&mutex);
            bathroom = EMPTY;
            printf("Bathroom is empty\n");
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}