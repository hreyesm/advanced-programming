
/*
  Solución al problema del productor-consumidor utilizando variables de condición
  - Buena solución porque no tiene espera activa
  - Además, brinda sincronización (a quien le corresponde el turno)
  - Funciones pthread_cond_wait() y pthread_cond_broadcast()
  - NP productores y NC consumidores
*/

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 10

#define G_USERS 10
#define B_USERS 5
#define ATMS 8

int lowerGUser = 1, upperGUser = 2;
int lowerEUser = 3, upperEUser = 4;
int lowerAtm = 5, upperAtm = 6;

int gQueue[BUFF_SIZE];
int bQueue[BUFF_SIZE];

int in = 0;
int out = 0;

int total = 0;

int usersCreated = 0, usersDispatched = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t consume_t = PTHREAD_COND_INITIALIZER;
pthread_cond_t produce_t = PTHREAD_COND_INITIALIZER;

void * agent(void *);
void * atms(void *);

double operation(int lower, int upper) {
    time_t start = time(NULL);
    sleep((rand() % (upper - lower + 1)) + lower);
    time_t end = time(NULL);
    return (double)(end - start);
}

int main(int argc, const char * argv[]) {  
    srand((int) time(NULL));
    
    int nThreads = ATMS + 1;
    
    pthread_t *bank = (pthread_t *) malloc (sizeof(pthread_t) * nThreads);
    
    int index = 0;
    
    pthread_t *aux = bank;
    
    /* Crear los productores */
    pthread_create(aux, NULL, agent, (void *) index);
    printf("Agent was created\n");

    ++aux;

    /* Crear los consumidores */    
    for (; aux < bank + nThreads; ++aux) {
        printf("ATM %d was created\n", ++index);
        pthread_create(aux, NULL, atms, (void *) index);
    }

    /* Adjuntar los hilos */
    for (aux = bank; aux < (bank + nThreads); ++aux) {
        pthread_join(*aux, NULL);
    }
    
    free(bank);
    
    return 0;

}

void *agent(void *arg) {
    int id = (int) arg;  
    printf("Agent starts\n");
    
    while (usersCreated < (G_USERS + B_USERS - 1)) {  
        usleep(rand() % 500);
        pthread_mutex_lock(&mutex);
        
        if (total < BUFF_SIZE) {     
            /* Client with a general operation */
            gQueue[in] = usersCreated;
            printf("+++ (Agent %d) Client %d has arrived to the queue\n", id, gQueue[in]);
            ++usersCreated;
            ++in;
            in %= BUFF_SIZE;
            ++total;
            if (total == 1) {
                pthread_cond_broadcast(&consume_t);
            }
        } else {
            /* El buffer está lleno, se va a dormir */        
            printf("-------------- Agent %d is asleep ------------\n", id);
            pthread_cond_wait(&produce_t, &mutex);
            printf("-------------- Agent %d is awake ------------\n", id);
        }
        
        pthread_mutex_unlock(&mutex);
        
    }
    
    pthread_exit(NULL);
}

void * atms(void * arg) {
    int id = (int) arg;
    printf("ATM %d has started working\n", id);

    while (usersDispatched < (G_USERS + B_USERS)) {    
        /* Asumir que trabajan a diferentes velocidades */
        usleep(rand() % 500);
        pthread_mutex_lock(&mutex);
        
        if (total > 0) { // Hay un elemento en el buffer
            /* Consume un elemento */
            printf("--- ATM %d dispatched client %d\n", id, gQueue[out]);
            ++usersDispatched;
            ++out;
            out %= BUFF_SIZE;
            --total;
            
            if (total == (BUFF_SIZE - 1) ) {
                pthread_cond_broadcast(&produce_t);
            }
            
        } else {
            /* El buffer está vacío, se va a dormir */
            printf("-------------- ATM %d is asleep ------------\n", id);
            pthread_cond_wait(&consume_t, &mutex);
            printf("-------------- ATM %d is awake ------------\n", id);
        }
        
        pthread_mutex_unlock(&mutex);
        
    }
    
    pthread_exit(NULL);
}