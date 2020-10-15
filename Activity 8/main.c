#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define BSIZE 9
#define N 21

#define G_CLIENTS 10
#define B_CLIENTS 11

#define G 0
#define B 1

#define PRODUCTORES 1
#define CONSUMIDORES 8

int gQueue[BSIZE];
int bQueue[BSIZE];

int gIn = 0;
int gOut = 0;

int bIn = 0;
int bOut = 0;

int gTotal = 0;
int bTotal = 0;

int producidos = 0, consumidos = 0;

pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_g = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t consume_b = PTHREAD_COND_INITIALIZER;
pthread_cond_t consume_g = PTHREAD_COND_INITIALIZER;
pthread_cond_t produce_t = PTHREAD_COND_INITIALIZER;

void * productor(void *);
void * consumidor(void *);

typedef struct {
    int id;
    int type;
} client;

int main(int argc, const char * argv[]) {    
    srand((int) time(NULL));
    int nhilos = PRODUCTORES + CONSUMIDORES;
    pthread_t * obreros = (pthread_t *) malloc (sizeof(pthread_t) * nhilos);
    int indice = 0;
    pthread_t * aux = obreros;
    /* Crear los productores */
    printf("(I) Creando el productor %d\n", ++indice);
    pthread_create(aux, NULL, productor, (void *) indice);
    aux += 1;
    /* Crear los consumidores */
    indice = 0;
    for (; aux < (obreros+nhilos); ++aux) {
        printf("(I) Creando el consumidor %d\n", ++indice);
        pthread_create(aux, NULL, consumidor, (void *) indice);
    }
    /* Adjuntar los hilos */
    for (aux = obreros; aux < (obreros+nhilos); ++aux) {
        pthread_join(*aux, NULL);
    }
    printf("\nGENERAL CLIENTS %d\nBUSINESS CLIENTS %d\n", gTotal, bTotal);
    free(obreros);
    
    return 0;
}

void *productor(void *arg) {
    int id = (intptr_t) arg;
    int createdGClients = 0;
    int createdBClients = 0;
    int clientId = 0;
    int type = 0;
    printf("(P) Inicia productor %d\n", id);
    while (createdGClients + createdBClients < N) {
        printf("Clientes creados %d \t bTotal = %d   gTotal = %d\n", createdGClients + createdBClients, bTotal, gTotal);

        if (gTotal == BSIZE) {      
            printf("-------------- Productor %d se durmió para el GENERAL ------------\n", id);
            pthread_cond_wait(&produce_t, &mutex_g);
            printf("-------------- Productor %d se despertó para el GENERAL ------------\n", id);
        }
        
        if (bTotal == BSIZE) {
            printf("-------------- Productor %d se durmió para el BUSINESS ------------\n", id);
            pthread_cond_wait(&produce_t, &mutex_b);
            printf("-------------- Productor %d se despertó para el BUSINESS ------------\n", id);
        }
        
        client c;
        type = rand() % 2;

        if (type == G) {
            if (gTotal == BSIZE) {
                continue;
            } else if (createdGClients < G_CLIENTS) {
                c.type = type;
                createdGClients++;
            } else if (createdBClients < B_CLIENTS) {
                c.type = B;
                createdBClients++;
            }
        } else if (type == B) {
            if (bTotal == BSIZE) {
                continue;
            } else if (createdBClients < B_CLIENTS) {
                c.type = type;
                createdBClients++;
            } else if (createdGClients < G_CLIENTS) {
                c.type = G;
                createdGClients++;
            }
        }

        sleep(1);
        c.id = ++clientId;
        
        if (c.type == G) {
            pthread_mutex_lock(&mutex_g);
            if (gTotal < BSIZE) {  
                sleep(1);
                gQueue[gIn] = c.id;
                printf("+++ (Productor %d) Se produjo el elemento %d de tipo GENERAL\n", id, gQueue[gIn]);            
                ++gIn;
                gIn %= BSIZE;
                ++gTotal;
                if (gTotal == 1) {
                    pthread_cond_broadcast(&consume_g);
                }
            } else if (createdGClients + createdBClients < N) {
                printf("-------------- Productor %d se durmió para el GENERAL ------------\n", id);
                pthread_cond_wait(&produce_t, &mutex_g);
                printf("-------------- Productor %d se despertó para el GENERAL ------------\n", id);
            }
            pthread_mutex_unlock(&mutex_g);
        } else if (c.type == B) {
            pthread_mutex_lock(&mutex_b);
            if (bTotal < BSIZE) {  
                sleep(1);
                bQueue[bIn] = c.id;
                printf("+++ (Productor %d) Se produjo el elemento %d de tipo BUSINESS\n", id, bQueue[bIn]);            
                ++bIn;
                bIn %= BSIZE;
                ++bTotal;
                if (bTotal == 1) {
                    pthread_cond_broadcast(&consume_b);
                }
            } else if (createdGClients + createdBClients < N) {
                printf("-------------- Productor %d se durmió para el BUSINESS ------------\n", id);
                pthread_cond_wait(&produce_t, &mutex_b);
                printf("-------------- Productor %d se despertó para el BUSINESS ------------\n", id);
            }
            pthread_mutex_unlock(&mutex_b);
        }
    }
    pthread_exit(NULL);
}

void *consumidor(void * arg) {
    int id = (intptr_t) arg;    
    printf("(C) Inicia consumidor %d\n", id);
    
    while (consumidos < N) {    
        if (id <= 5) { // General ATM
            pthread_mutex_lock(&mutex_g);
            if (gTotal > 0) {      
                printf("--- (GENERAL ATM %d) Se consumió el elemento %d\n", id, gQueue[gOut]);
                sleep(1);
                ++consumidos;            
                ++gOut;
                gOut %= BSIZE;
                --gTotal;
                if (gTotal == BSIZE - 1) {
                    pthread_cond_broadcast(&produce_t);
                }
            } else if (consumidos < N) {
                printf("-------------- GENERAL ATM %d se durmió ------------\n", id);
                pthread_cond_wait(&consume_g, &mutex_g);
                printf("-------------- GENERAL ATM %d se despertó ------------\n", id);
            }
            pthread_mutex_unlock(&mutex_g);
        } else { // Business ATM
            pthread_mutex_lock(&mutex_b);
            if (bTotal > 0) {
                printf("--- (BUSINESS ATM %d) Se consumió el elemento %d\n", id, bQueue[bOut]);
                sleep(1);
                ++consumidos;            
                ++bOut;
                bOut %= BSIZE;
                --bTotal;
                if (bTotal == BSIZE - 1) {
                    pthread_cond_broadcast(&produce_t);
                }
            } else if (consumidos < N) {
                printf("-------------- BUSINESS ATM %d se durmió ------------\n", id);
                pthread_cond_wait(&consume_b, &mutex_b);
                printf("-------------- BUSINESS ATM %d se despertó ------------\n", id);
            }
            pthread_mutex_unlock(&mutex_b);
        }
    } 
    pthread_exit(NULL);
}