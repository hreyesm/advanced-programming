// Ejercicio 1

// Blancanieves y los siete enanitos viven en una casa donde solo existen cuatro sillas, que los enanitos utilizan para comer.

// Cuando un enanito vuelve de trabajar en la mina, comprueba si hay una silla libre para sentarse.
// Si existe una silla libre, entonces indica a Blancanieves que ya está sentado, y espera pacientemente su turno a que le sirvan.
// Cuando le ha servido, Blancanieves le indica que puede empezar a comer.
// El enanito come y cuando acaba, deja la silla libre y vuelve a la mina.
// Por su parte, Blancanieves cuando no tiene ningún enanito pendiente de servirle, se va a dar una vuelta.

// Realice un análisis exhaustivo de los problemas de concurrencia que se pueden dar en un sistema de este tipo, planteando cada uno de ellos y explicándolo.
// Seleccione una técnica de concurrencia para solucionar el problema, explicando los motivos de su selección.
// Partiendo del análisis realizado y la técnica de concurrencia seleccionada, programe una solución que resuelva el problema descrito. Utilizar únicamente dos tipos de procesos: Enanito y Blancanieves.

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define CHAIRS 4
#define DWARFS 7

int table[CHAIRS];

int in = 0;
int out = 0;

int occupiedChairs = 0;
int dwarfsServed = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t swSemaphore, dSemaphore;

void *dwarf(void *);
void *snowWhite(void *);

int main(int argc, const char *argv[]) {
    srand((unsigned int) time(NULL));
    sem_init(&swSemaphore, 0, 0);
    sem_init(&dSemaphore, 0, 0);
    int dwarfId = 0;     
    pthread_t sw;
    pthread_t *d = (pthread_t *) malloc(sizeof(pthread_t) * DWARFS);
    pthread_t *aux;
    pthread_create(&sw, NULL, snowWhite, NULL);
    for (aux = d; aux < d + DWARFS; ++aux) {
        ++dwarfId;
        pthread_create(aux, NULL, dwarf, (void *) dwarfId);
    }
    pthread_join(sw, NULL);
    for (aux = d; aux < d + DWARFS; ++aux) {
        pthread_join(*aux, NULL);  
    }
    sem_destroy(&swSemaphore);
    sem_destroy(&dSemaphore);
    free(d);
    pthread_exit(NULL);
}

void *dwarf(void *arg) {
    int id = (intptr_t) arg;
    while (dwarfsServed < DWARFS) {
        usleep(rand() % 300);
        if (occupiedChairs < CHAIRS) {
            pthread_mutex_lock(&mutex);
            ++occupiedChairs;
            table[in] = id;
            printf("DWARF %d has sat at the table (%d occupied chairs)\n", table[in], occupiedChairs);
            ++in;
            in %= CHAIRS;
            sem_post(&swSemaphore);  // le indica a sw que ya se sentó
            pthread_mutex_unlock(&mutex);
            sem_wait(&dSemaphore);   // sw le indica que ya puede comer
            printf("DWARF %d has started eating\n", id);
            usleep(rand() % 300);
            // pthread_mutex_lock(&mutex);
            --occupiedChairs;
            ++dwarfsServed;
            // pthread_mutex_unlock(&mutex);
            printf("DWARF %d has finished eating and left the table (%d occupied chairs) \n", id, occupiedChairs);
            pthread_exit(NULL);
        } else {
             printf("DWARF %d cannot sit at the table (%d occupied chairs)\n", id, occupiedChairs);
        }
    }
}

void *snowWhite(void *arg) {
    int i = 0;
    int value;
    while (i < DWARFS) {
        sem_wait(&swSemaphore);
        printf("SNOW WHITE has served DWARF %d\n", table[out]);
        ++i;
        ++out;
        out %= CHAIRS;
        sem_post(&dSemaphore); // le indica al dwarf que ya puede comer
        if (occupiedChairs == 0) {
            printf("SNOW WHITE has gone for a walk as there are no more dwarfs to serve\n");
        }
    }
    pthread_exit(NULL);
}