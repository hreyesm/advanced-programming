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

void *womenInBathroom(void *);
void *menInBathroom(void *);

int main(int argc, const char *argv[]) {
    srand((int) time(NULL));  
    pthread_t *people = (pthread_t *) malloc (sizeof(pthread_t) * PEOPLE);
    int peopleCreated = 0;
    int gender = 0;
    if (peopleInBathroom == 0) {
        printf("Bathroom is empty\n");
    }
    while (peopleCreated < PEOPLE) {
        gender = rand() % 2;
        if (gender == 0) {
            pthread_create(people + peopleCreated, NULL, womenInBathroom, NULL);
        } else {
            pthread_create(people + peopleCreated, NULL, menInBathroom, NULL);
        }
        ++peopleCreated;
    }
    for (pthread_t *aux = people; aux < people + PEOPLE; ++aux) {
        pthread_join(*aux, NULL);
    }
    free(people);
    return 0;
}

void *womenInBathroom(void *id) {
    pthread_mutex_lock(&mutex);
    printf("(W) WOMAN has arrived to the queue (%d waiting)\n", ++womenInQueue);
    while (womenInQueue > 0) {
        if (peopleInBathroom == 0 || women > 0) {
            printf("(W) --> WOMAN has entered the bathroom (%d waiting)\n", --womenInQueue);
            ++women;
            ++peopleInBathroom;
            pthread_mutex_unlock(&mutex);
            usleep(300);
            pthread_mutex_lock(&mutex);
            printf("(W) WOMAN has left the bathroom -->\n");
            --women;
            --peopleInBathroom;
            if (peopleInBathroom == 0) {
                printf("Bathroom is empty\n");
                pthread_cond_broadcast(&mutex_m);
            }        
        } else {
            printf("(W) WOMAN cannot enter the bathroom\n");
            pthread_cond_wait(&mutex_w, &mutex);
            printf("(W) WOMAN can enter the bathroom\n");
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *menInBathroom(void *id) {
    pthread_mutex_lock(&mutex);
    printf("(M) MAN has arrived to the queue (%d waiting)\n", ++menInQueue);
    while (menInQueue > 0) {    
        if (peopleInBathroom == 0 || men > 0) {
            printf("(M) --> MAN has entered the bathroom (%d waiting)\n", --menInQueue);
            ++men;
            ++peopleInBathroom;
            pthread_mutex_unlock(&mutex);
            usleep(300);        
            pthread_mutex_lock(&mutex);
            printf("(M) MAN has left the bathroom -->\n");
            --men;
            --peopleInBathroom;
            if (peopleInBathroom == 0) {
                printf("Bathroom is empty\n");
                pthread_cond_broadcast(&mutex_w);
            }
        } else {        
            printf("(M) MAN cannot enter the bathroom\n");
            pthread_cond_wait(&mutex_m, &mutex);
            printf("(M) MAN can enter the bathroom\n");
        }
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}