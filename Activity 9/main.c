#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define PEOPLE 100

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mutex_w = PTHREAD_COND_INITIALIZER;
pthread_cond_t mutex_m = PTHREAD_COND_INITIALIZER;
sem_t womenSemaphoreIn, womenSemaphoreOut, menSemaphoreIn, menSemaphoreOut;

int women = 0;
int men = 0;
int peopleInBathroom = 0;
int womenInQueue = 0, menInQueue = 0;

void *womanEnters(void *);
void *manEnters(void *);
void *womanLeaves(void *);
void *manLeaves(void *);

int main(int argc, const char *argv[]) {
    srand((int) time(NULL));  
    pthread_t *people = (pthread_t *) malloc (sizeof(pthread_t) * PEOPLE * 2);
    int peopleCreated = 0;
    int gender = 0;
    int womenIndex = 0;
    int menIndex = 0;
    sem_init(&womenSemaphoreIn, 0, 1);
    sem_init(&womenSemaphoreOut, 0, 0);
    sem_init(&menSemaphoreIn, 0, 1);
    sem_init(&menSemaphoreOut, 0, 0);
    if (peopleInBathroom == 0) {
        printf("Bathroom is empty\n");
    }
    while (peopleCreated < PEOPLE * 2) {
        gender = rand() % 2;
        if (gender == 0) {
            pthread_create(people + peopleCreated, NULL, womanEnters, NULL);
            ++peopleCreated;
            pthread_create(people + peopleCreated, NULL, womanLeaves, NULL);
        } else {
            pthread_create(people + peopleCreated, NULL, manEnters, NULL);
            ++peopleCreated;
            pthread_create(people + peopleCreated, NULL, manLeaves, NULL);
        }
        ++peopleCreated;
    }
    for (pthread_t *aux = people; aux < people + PEOPLE * 2; ++aux) {
        pthread_join(*aux, NULL);
    }
    free(people);
    sem_destroy(&womenSemaphoreIn);
    sem_destroy(&womenSemaphoreOut);
    sem_destroy(&menSemaphoreIn);
    sem_destroy(&menSemaphoreOut);
    return 0;
}

void *womanEnters(void *id) {
    sem_wait(&womenSemaphoreIn);
    printf("(W) WOMAN has arrived to the queue (%d waiting)\n", ++womenInQueue);
    while (womenInQueue > 0) {
        if (peopleInBathroom == 0 || women > 0) {
            printf("(W) --> WOMAN has entered the bathroom (%d waiting)\n", --womenInQueue);
            ++women;
            ++peopleInBathroom;
            sem_post(&womenSemaphoreOut);
            usleep(300);
        } else {
            printf("(W) WOMAN cannot enter the bathroom\n");
            sem_post(&menSemaphoreIn);
            sem_wait(&womenSemaphoreIn);
            printf("(W) WOMAN can enter the bathroom\n");
        }
    }
}

void *manEnters(void *id) {
    sem_wait(&menSemaphoreIn);
    printf("(M) MAN has arrived to the queue (%d waiting)\n", ++menInQueue);
    while (menInQueue > 0) {
        if (peopleInBathroom == 0 || men > 0) {
            printf("(M) --> MAN has entered the bathroom (%d waiting)\n", --menInQueue);
            ++men;
            ++peopleInBathroom;
            sem_post(&menSemaphoreOut);
            usleep(300);
        } else {
            printf("(M) MAN cannot enter the bathroom\n");
            sem_post(&womenSemaphoreIn);
            sem_wait(&menSemaphoreIn);
            printf("(M) MAN can enter the bathroom\n");
        }
    }
}

void *womanLeaves(void *id) {
    sem_wait(&womenSemaphoreOut);
    while (women > 0) {
        printf("(W) WOMAN has left the bathroom -->\n");
        --women;
        --peopleInBathroom;
        if (peopleInBathroom == 0) {
            printf("Bathroom is empty\n");
            sem_post(&menSemaphoreIn);
        }
    }
}

void *manLeaves(void *id) {
    sem_wait(&menSemaphoreOut);
    while (men > 0) {
        printf("(M) MAN has left the bathroom -->\n");
        --men;
        --peopleInBathroom;
        if (peopleInBathroom == 0) {
            printf("Bathroom is empty\n");
            sem_post(&womenSemaphoreIn);
        }
    }
}