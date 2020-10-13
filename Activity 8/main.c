#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define ATMS 8
#define G_USERS 10
#define E_USERS 5

int lowerAtm = 3, upperAtm = 5;
int lowerGUser = 1, upperGUser = 2;
int lowerEUser = 3, upperEUser = 4;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double operation(int lower, int upper) {
    time_t start = time(NULL);
    sleep((rand() % (upper - lower + 1)) + lower);
    time_t end = time(NULL);
    return (double)(end - start);
}

void *generalOps (void * arg) {
    int tid = (int) arg;
    int i;
    double totalTime = 0.0;

    pthread_mutex_lock(&mutex);
    totalTime = operation(lowerAtm, upperAtm);
    printf("GO - User %d, Time spent: %.2f\n", tid, totalTime);
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

void *businessOps (void * arg) {
    int tid = (int) arg;
    int i;
    double totalTime = 0.0;

    pthread_mutex_lock(&mutex);
    totalTime = operation(lowerAtm, upperAtm);
    printf("BO - User %d, Time spent: %.2f\n", tid, totalTime);
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(0);
}

int main(int argc, const char * argv[]) {
    pthread_t *gid, *eid;
    int nATMs, gUsers, eUsers, i;
    double totalTime;
    
    srand((unsigned int) time(NULL));
    
    nATMs = ATMS;
    gUsers = G_USERS;
    eUsers = E_USERS;
    
    gid = malloc(gUsers * sizeof(pthread_t));
    eid = malloc(eUsers * sizeof(pthread_t));
    
    printf("The bank is open...\n");
    
    for (i = 0; i < gUsers; i++) {
        totalTime = operation(lowerGUser, upperGUser);
        printf("User %d, Arrival time: %.2f\n", i, totalTime);
        pthread_create(gid + i, NULL, generalOps, (void *) i);
    }

    for (i = 0; i < eUsers; i++) {
        totalTime = operation(lowerEUser, upperEUser);
        printf("User %d, Arrival time: %.2f\n", i + gUsers, totalTime);
        pthread_create(eid + i, NULL, businessOps, (void *) i + gUsers);
    }

    printf("%d clients were attended...\n", gUsers + eUsers);

    for (i = 0; i < gUsers; i++) {
        pthread_join(*(gid + i), NULL);
    }

    for (i = 0; i < eUsers; i++) {
        pthread_join(*(eid + i), NULL);
    }    
    
    printf("The bank is closed..\n");
    
    free(gid);
    free(eid);
    
    return 0;
}