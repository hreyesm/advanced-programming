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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t womenSemaphore, menSemaphore;

int womenProduced = 0, womenConsumed = 0;
int menProduced = 0, menConsumed = 0;
int bathroom = 0;

void *womanEnters(void *);
void *manEnters(void *);
void *womanLeaves(void *);
void *manLeaves(void *);

int main(int argc, const char *argv[]) {
    srand((int) time(NULL));  

    pthread_t threads[4];
    
    sem_init(&womenSemaphore, 0, 0);
    sem_init(&menSemaphore, 0, 0);
    
    if (bathroom == EMPTY)
        printf("Bathroom is empty\n");

    pthread_create(&threads[0], NULL, &womanEnters, NULL);
    pthread_create(&threads[1], NULL, &womanLeaves, NULL);
    pthread_create(&threads[2], NULL, &manEnters, NULL);
    pthread_create(&threads[3], NULL, &manLeaves, NULL);
    
    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    sem_destroy(&womenSemaphore);
    sem_destroy(&menSemaphore);

    pthread_exit(NULL);
}

void *womanEnters(void *arg) {
    int womenInQueue = 0;
    while (womenProduced + menProduced < TOTAL) {
        // usleep(rand() % 500);
        printf("(W) WOMAN has arrived to the queue (%d waiting)\n", ++womenInQueue);
        ++womenProduced;
        pthread_mutex_lock(&mutex);
        if (bathroom != MEN) {
            if (bathroom == EMPTY)
                bathroom = WOMEN;
            if (bathroom == WOMEN) {
                printf("(W) --> WOMAN has entered the bathroom (%d waiting)\n", --womenInQueue);
                sem_post(&womenSemaphore);
            }
        } else {
            printf("(W) WOMAN cannot enter the bathroom\n");
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *manEnters(void *arg) {
    int menInQueue = 0;
    while (womenProduced + menProduced < TOTAL) {
        // usleep(rand() % 500);
        printf("(M) MAN has arrived to the queue (%d waiting)\n", ++menInQueue);
        ++menProduced;
        pthread_mutex_lock(&mutex);
        if (bathroom != WOMEN) {
            if (bathroom == EMPTY)
                bathroom = MEN;
            if (bathroom == MEN) {
                printf("(M) --> MAN has entered the bathroom (%d waiting)\n", --menInQueue);
                sem_post(&menSemaphore);
            }
        } else {
            printf("(M) MAN cannot enter the bathroom\n");
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *womanLeaves(void *arg) {
    int womenInBathroom;
    while (womenConsumed + menConsumed < TOTAL) {
        sem_wait(&womenSemaphore);
        sem_getvalue(&womenSemaphore, &womenInBathroom);
        printf("(W) WOMAN has left the bathroom (%d inside) -->\n", womenInBathroom);
        ++womenConsumed;
        if (!womenInBathroom) {
            // usleep(rand() % 500);
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
    while (womenConsumed + menConsumed < TOTAL) {
        sem_wait(&menSemaphore);
        sem_getvalue(&menSemaphore, &menInBathroom);
        printf("(W) MAN has left the bathroom (%d inside) -->\n", menInBathroom);
        ++menConsumed;
        if (!menInBathroom) {
            // usleep(rand() % 500);
            pthread_mutex_lock(&mutex);
            bathroom = EMPTY;
            printf("Bathroom is empty\n");
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}