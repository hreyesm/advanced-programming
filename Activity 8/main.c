#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 5

#define N 25
#define G_CLIENTS 15
#define B_CLIENTS 10

#define G 0
#define B 1

#define PRODUCTORES 1
#define CONSUMIDORES 8

int gQueue[BUFF_SIZE], bQueue[BUFF_SIZE];
int gIn = 0, gOut = 0;
int bIn = 0, bOut = 0;
int gTotal = 0, bTotal = 0;

int producidos = 0, consumidos_g = 0, consumidos_b = 0;

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
    int indice = 0;
    int nhilos = PRODUCTORES + CONSUMIDORES;
    pthread_t * obreros = (pthread_t *) malloc (sizeof(pthread_t) * nhilos);
    pthread_t * aux = obreros;

    printf("(I) Creando el productor %d\n", ++indice);
    pthread_create(aux, NULL, productor, (void *) indice);

    aux += 1;
    indice = 0;

    for (; aux < (obreros+nhilos); ++aux) {
        printf("(I) Creando el consumidor %d\n", ++indice);
        pthread_create(aux, NULL, consumidor, (void *) indice);
    }

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
        client c;
        type = rand() % 2;
        c.type = type;
        // sleep(1);
        if (c.type == G && createdGClients < G_CLIENTS) {
            pthread_mutex_lock(&mutex_g);
            c.id = ++clientId;
            if ( gTotal  < BUFF_SIZE && createdGClients < G_CLIENTS ) {  
                // sleep(1);
                gQueue[gIn] = c.id;
                printf("+++ (Productor %d) Se produjo el elemento %d de tipo GENERAL\n", id, gQueue[gIn]);            
                ++gIn;
                gIn %= BUFF_SIZE;
                createdGClients++;
                ++gTotal;
                if (gTotal == 1) {
                    pthread_cond_broadcast(&consume_g);
                }
            } else if (createdGClients < G_CLIENTS) {
                printf("-------------- Productor %d se durmió para la cola GENERAL ------------\n", id);
                pthread_cond_wait(&produce_t, &mutex_g);
                printf("-------------- Productor %d se despertó para la cola GENERAL ------------\n", id);
            }

            pthread_mutex_unlock(&mutex_g);
        } else if (c.type == B && createdBClients < B_CLIENTS ) {
            pthread_mutex_lock(&mutex_b);
            c.id = ++clientId;
            if ( bTotal  < BUFF_SIZE && createdBClients < B_CLIENTS) {  
                // sleep(1);
                bQueue[bIn] = c.id;
                printf("+++ (Productor %d) Se produjo el elemento %d de tipo BUSINESS\n", id, bQueue[bIn]);            
                ++bIn;
                bIn %= BUFF_SIZE;
                createdBClients++;
                ++bTotal;
                if (bTotal == 1) {
                    pthread_cond_broadcast(&consume_b);
                }
            } else if (createdBClients < B_CLIENTS) {
                printf("-------------- Productor %d se durmió para la cola BUSINESS ------------\n", id);
                pthread_cond_wait(&produce_t, &mutex_b);
                printf("-------------- Productor %d se despertó para la cola BUSINESS ------------\n", id);
            }

            pthread_mutex_unlock(&mutex_b);
        }
    }
    pthread_exit(NULL);
}

void *consumidor(void * arg) {
    int id = (intptr_t) arg;    
    printf("(C) Inicia consumidor %d\n", id);

    while ((consumidos_g + consumidos_b) < N) {    
        if (id <= 5) { // General ATM
            pthread_mutex_lock(&mutex_g);
            if (gTotal > 0 && consumidos_g < G_CLIENTS ) {      
                printf("--- (GENERAL ATM %d) Se consumió el elemento %d\n", id, gQueue[gOut]);
                // sleep(1);
                ++consumidos_g;            
                ++gOut;
                gOut %= BUFF_SIZE;
                --gTotal;
                if (gTotal == BUFF_SIZE - 1) {
                    pthread_cond_broadcast(&produce_t);
                }
            } else if (consumidos_g < G_CLIENTS) {
                printf("-------------- GENERAL ATM %d se durmió ------------\n", id);
                pthread_cond_wait(&consume_g, &mutex_g);
                printf("-------------- GENERAL ATM %d se despertó ------------\n", id);
            }
            pthread_mutex_unlock(&mutex_g);
        } else { // Business ATM
            pthread_mutex_lock(&mutex_b);
            if (bTotal > 0 && consumidos_b <  B_CLIENTS) {
                printf("--- (BUSINESS ATM %d) Se consumió el elemento %d\n", id, bQueue[bOut]);
                // sleep(1);
                ++consumidos_b;            
                ++bOut;
                bOut %= BUFF_SIZE;
                --bTotal;
                if (bTotal == BUFF_SIZE - 1) {
                    pthread_cond_broadcast(&produce_t);
                }
            } else if (consumidos_b < B_CLIENTS) {
                printf("-------------- BUSINESS ATM %d se durmió ------------\n", id);
                pthread_cond_wait(&consume_b, &mutex_b);
                printf("-------------- BUSINESS ATM %d se despertó ------------\n", id);
            }
            pthread_mutex_unlock(&mutex_b);
        }
    } 
    pthread_exit(NULL);
}