#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define PEOPLE 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mutex_w = PTHREAD_COND_INITIALIZER;
pthread_cond_t mutex_m = PTHREAD_COND_INITIALIZER;

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
    if (peopleInBathroom == 0) {
        printf("Bathroom is empty\n");
    }
    while (peopleCreated < PEOPLE * 2) {
        gender = rand() % 2;
        if (gender == 0) {
            pthread_create(people + peopleCreated, NULL, womanEnters, (void *) ++womenIndex);
            ++peopleCreated;
            pthread_create(people + peopleCreated, NULL, womanLeaves, (void *) womenIndex);
        } else {
            pthread_create(people + peopleCreated, NULL, manEnters, (void *) ++menIndex);
            ++peopleCreated;
            pthread_create(people + peopleCreated, NULL, manLeaves, (void *) menIndex);
        }
        ++peopleCreated;
    }
    for (pthread_t *aux = people; aux < people + PEOPLE * 2; ++aux) {
        pthread_join(*aux, NULL);
    }
    free(people);
    return 0;
}

void *womanEnters(void *id) {
    int index = (intptr_t) id;
    pthread_mutex_lock(&mutex);
    printf("(W) WOMAN %d has arrived to the queue (%d waiting)\n", index, ++womenInQueue);
    while (womenInQueue > 0) {
        if (peopleInBathroom == 0 || women > 0) {
            printf("(W) --> WOMAN %d has entered the bathroom (%d waiting)\n", index, --womenInQueue);
            ++women;
            ++peopleInBathroom;
            pthread_mutex_unlock(&mutex);
            usleep(300);
        } else {
            printf("(W) WOMAN % d cannot enter the bathroom\n", index);
            pthread_cond_wait(&mutex_w, &mutex);
            printf("(W) WOMAN %d can enter the bathroom\n", index);
        }
    }
    pthread_exit(NULL);
}

void *manEnters(void *id) {
    int index = (intptr_t) id;
    pthread_mutex_lock(&mutex);
    printf("(M) MAN %d has arrived to the queue (%d waiting)\n", index, ++menInQueue);
    while (menInQueue > 0) {
        if (peopleInBathroom == 0 || men > 0) {
            printf("(M) --> MAN %d has entered the bathroom (%d waiting)\n", index, --menInQueue);
            ++men;
            ++peopleInBathroom;
            pthread_mutex_unlock(&mutex);
            usleep(300);
        } else {
            printf("(M) MAN %d cannot enter the bathroom\n", index);
            pthread_cond_wait(&mutex_m, &mutex);
            printf("(M) MAN %d can enter the bathroom\n", index);
        }
    }
    pthread_exit(NULL);
}

void *womanLeaves(void *id) {
    int index = (intptr_t) id;
    pthread_mutex_lock(&mutex);
    while (women > 0) {
        printf("(W) WOMAN %d has left the bathroom -->\n", index);
        --women;
        --peopleInBathroom;
        if (peopleInBathroom == 0) {
            printf("Bathroom is empty\n");
            pthread_cond_broadcast(&mutex_m);
        }
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void *manLeaves(void *id) {
    int index = (intptr_t) id;
    pthread_mutex_lock(&mutex);
    while (men > 0) {
        printf("(M) MAN %d has left the bathroom -->\n", index);
        --men;
        --peopleInBathroom;
        if (peopleInBathroom == 0) {
            printf("Bathroom is empty\n");
            pthread_cond_broadcast(&mutex_w);
        }
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}