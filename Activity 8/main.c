/*  Actividad 8: Concurrencia
    Curso: Programación avanzada
    Profesor: Vicente Cubells Nonell
    Autores:
        Daniela Vignau León (A01021698)
        Héctor Alexis Reyes Manrique (A01339607)
    Fecha: 16 de octubre de 2020
*/

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 10

#define N 150
#define GENERAL_CLIENTS 100
#define BUSINESS_CLIENTS 50

#define GENERAL 0
#define BUSINESS 1

#define ATMS 8

int generalQueue[BUFF_SIZE], businessQueue[BUFF_SIZE];
int gIn = 0, gOut = 0;
int bIn = 0, bOut = 0;
int gTotal = 0, bTotal = 0;

int gClientsServed = 0, bClientsServed = 0;

int gClientLower = 5, gClientUpper = 22;
int bClientLower = 9, bClientUpper = 34;
int atmLower = 3, atmUpper = 5;

typedef struct {
    int id;
    int type;
} client;

pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_g = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t consume_b = PTHREAD_COND_INITIALIZER;
pthread_cond_t consume_g = PTHREAD_COND_INITIALIZER;
pthread_cond_t produce_t = PTHREAD_COND_INITIALIZER;

int operation(int lower, int upper) {
    time_t start = time(NULL);
    sleep((rand() % (upper - lower + 1)) + lower);
    time_t end = time(NULL);
    return (int) (end - start);
}

void *agent(void *arg) {
    int gClientsCreated = 0, bClientsCreated = 0, clientId = 0, type = 0, totalTime;
    while (gClientsCreated + bClientsCreated < N) {
        client c;
        type = rand() % 2;
        c.type = type;
        if (c.type == GENERAL && gClientsCreated < GENERAL_CLIENTS) {
                pthread_mutex_lock(&mutex_g);
                c.id = ++clientId;
                if (gTotal < BUFF_SIZE && gClientsCreated < GENERAL_CLIENTS) {  
                    totalTime = (int) operation(gClientLower, gClientUpper);
                    sleep(totalTime);
                    generalQueue[gIn] = c.id;
                    printf("+++ (Agent) Client with ID %d arrived to the GENERAL queue in %d seconds\n", generalQueue[gIn], totalTime);
                    ++gIn;
                    gIn %= BUFF_SIZE;
                    gClientsCreated++;
                    ++gTotal;
                    if (gTotal == 1) {
                        pthread_cond_broadcast(&consume_g);
                        pthread_cond_broadcast(&consume_b);
                    }
                } else if (gClientsCreated < GENERAL_CLIENTS) {
                    printf("-------------- Agent has stopped adding new clients to the GENERAL queue ------------\n");
                    pthread_cond_wait(&produce_t, &mutex_g);
                    printf("-------------- Agent has resumed adding new clients to the GENERAL queue ------------\n");
                }
                pthread_mutex_unlock(&mutex_g);
        } else if (c.type == BUSINESS && bClientsCreated < BUSINESS_CLIENTS ) {
            pthread_mutex_lock(&mutex_b);
            c.id = ++clientId;
            if (bTotal < BUFF_SIZE && bClientsCreated < BUSINESS_CLIENTS) {  
                totalTime = operation(bClientLower, bClientUpper);
                sleep(totalTime);
                businessQueue[bIn] = c.id;
                printf("+++ (Agent) Client with ID %d arrived to the BUSINESS queue in %d seconds\n", businessQueue[bIn], totalTime);
                ++bIn;
                bIn %= BUFF_SIZE;
                bClientsCreated++;
                ++bTotal;
                if (bTotal == 1) {
                    pthread_cond_broadcast(&consume_b);
                }
            } else if (bClientsCreated < BUSINESS_CLIENTS) {
                printf("-------------- Agent has stopped adding new clients to the BUSINESS queue ------------\n");
                pthread_cond_wait(&produce_t, &mutex_b);
                printf("-------------- Agent has resumed adding new clients to the BUSINESS queue ------------\n");
            }
            pthread_mutex_unlock(&mutex_b);
        }
    }
    pthread_exit(NULL);
}

void *atm(void *arg) {
    int id = (intptr_t) arg;
    int counter = 0, totalTime = 0, sleepTime = 3;
    while (gClientsServed + bClientsServed < N) {    
        if (id <= 5) {
            pthread_mutex_lock(&mutex_g);
            if (gTotal > 0 && gClientsServed < GENERAL_CLIENTS ) {
                printf("---> (GENERAL ATM %d) is serving GENERAL client with ID %d\n", id, generalQueue[gOut]);
                totalTime = operation(atmLower, atmUpper);
                sleep(totalTime);      
                printf("<--- (GENERAL ATM %d) has served GENERAL client with ID %d in %d seconds\n", id, generalQueue[gOut], totalTime);
                ++gClientsServed;            
                ++gOut;
                gOut %= BUFF_SIZE;
                --gTotal;
                counter++;
                if (gTotal == BUFF_SIZE - 1) {
                    pthread_cond_broadcast(&produce_t);
                }
                if (counter == 5) {
                    counter = 0;
                    printf("zzz - (GENERAL ATM %d) stopped operations for %d seconds\n", id, sleepTime);
                    sleep(sleepTime);
                    printf("aaa - (GENERAL ATM %d) has resumed operations\n", id);
                }
            } else if (gClientsServed < GENERAL_CLIENTS) {
                pthread_cond_wait(&consume_g, &mutex_g);
            }
            pthread_mutex_unlock(&mutex_g);
        } else {
            pthread_mutex_lock(&mutex_b);
            if (bTotal > 0 && bClientsServed <  BUSINESS_CLIENTS) {
                printf("---> (BUSINESS ATM %d) is serving BUSINESS client with ID %d\n", id, businessQueue[bOut]);
                totalTime = operation(atmLower, atmUpper);
                sleep(totalTime);
                printf("<--- (BUSINESS ATM %d) has served BUSINES client with ID %d in %d seconds\n", id, businessQueue[bOut], totalTime);
                ++bClientsServed;            
                ++bOut;
                bOut %= BUFF_SIZE;
                --bTotal;
                counter++;
                if (bTotal == BUFF_SIZE - 1) {
                    pthread_cond_broadcast(&produce_t);
                }
                if (counter == 5) {
                    counter = 0;
                    printf("zzz - (BUSINESS ATM %d) stopped operations for %d seconds\n", id, sleepTime);
                    sleep(sleepTime);
                    printf("aaa - (BUSINESS ATM %d) has resumed operations\n", id);
                }                
            } else if (gTotal > 0 && gClientsServed < GENERAL_CLIENTS) {
                pthread_mutex_lock(&mutex_g);
                printf("---> (BUSINESS ATM %d) is serving GENERAL client with ID %d\n", id, generalQueue[gOut]);
                totalTime = operation(atmLower, atmUpper);
                sleep(totalTime);
                printf("<--- (BUSINESS ATM %d) has served GENERAL client with ID %d in %d seconds\n", id, generalQueue[gOut], totalTime);
                ++gClientsServed;            
                ++gOut;
                gOut %= BUFF_SIZE;
                --gTotal;
                counter++;
                if (gTotal == BUFF_SIZE - 1) {
                    pthread_cond_broadcast(&produce_t);
                }
                if (counter == 5) {
                    counter = 0;
                    printf("zzz - (BUSINESS ATM %d) stopped operations for %d seconds\n", id, sleepTime);
                    sleep(sleepTime);
                    printf("aaa - (BUSINESS ATM %d) has resumed operations\n", id);
                }
                pthread_mutex_unlock(&mutex_g);
            } else if (bClientsServed < BUSINESS_CLIENTS) {
                pthread_cond_wait(&consume_b, &mutex_b);
            }
            pthread_mutex_unlock(&mutex_b);
        }
    } 
    pthread_exit(NULL);
}

int main(int argc, const char *argv[]) {    
    srand((int) time(NULL));
    int index = 0;
    int nThreads = ATMS + 1;
    pthread_t * bank = (pthread_t *) malloc (sizeof(pthread_t) * nThreads);
    pthread_t * aux = bank;

    printf("BANK IS OPEN\n");
    pthread_create(aux, NULL, agent, (void *) index);

    aux += 1;
    index = 0;

    for (; aux < bank + nThreads; ++aux) {
        ++index;
        pthread_create(aux, NULL, atm, (void *) index);
    }

    for (aux = bank; aux < bank + nThreads; ++aux) {
        pthread_join(*aux, NULL);
    }

    free(bank);
    printf("\nBANK CLOSED\n");
    return 0;
}