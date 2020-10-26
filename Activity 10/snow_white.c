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

#define CHAIRS 4
#define DWARFS 7

int table[CHAIRS];

int in = 0;
int out = 0;

int occupiedChairs = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t produce_t = PTHREAD_COND_INITIALIZER;
pthread_cond_t consume_t = PTHREAD_COND_INITIALIZER;

void *dwarf(void *);
void *snowWhite(void *);

int main(int argc, const char * argv[]) {
    srand((unsigned int) time(NULL));
    pthread_t d, s;
    
    pthread_create(&d, NULL, dwarf, NULL);
    pthread_create(&s, NULL, snowWhite, NULL);
    pthread_join(d, NULL);  
    pthread_join(s, NULL);
    
    return 0;
}

void *dwarf(void *arg) {
    int i = 0;
    while (i < DWARFS) {
        usleep(rand() % 500);
        pthread_mutex_lock(&mutex);
        if (occupiedChairs < CHAIRS) {
            table[in] = i;
            printf("DWARF %d has sat at the table\n", table[in]);
            ++i;
            ++in;
            in %= CHAIRS;
            ++occupiedChairs;
            if (occupiedChairs == 1) {
                pthread_cond_signal(&consume_t);
            }
        } else {
            printf("DWARF %d cannot sit\n", i);
            pthread_cond_wait(&produce_t, &mutex);
            printf("DWARF %d can sit\n", i);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *snowWhite(void *arg) {
    int i = 0;
    while (i < DWARFS) {
        usleep(rand() % 500);
        pthread_mutex_lock(&mutex);
        if (occupiedChairs > 0) {
            printf("SNOW WHITE has served DWARF %d\n", table[out]);
            ++i;
            ++out;
            out %= CHAIRS;
            --occupiedChairs;
            if (occupiedChairs == CHAIRS - 1) {
                pthread_cond_signal(&produce_t);
            }
        } else {
            printf("SNOW WHITE has gone for a walk as there are no more dwarfs to serve\n");
            pthread_cond_wait(&consume_t, &mutex);
            printf("SNOW WHITE is back as there is a dwarf waiting to be served\n");
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}