/*  Actividad 10: Concurrencia (Ejercicio 1)
    Curso: Programación avanzada
    Profesor: Vicente Cubells Nonell
    Autores:
        Daniela Vignau León (A01021698)
        Héctor Alexis Reyes Manrique (A01339607)
    Fecha: 29 de octubre de 2020
*/

#include <stdio.h>
#include <pthread.h>.
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define CHAIRS 4
#define DWARFS 7

int table[CHAIRS];

int in = 0;
int out = 0;

int dwarfsArrived = 0;
int dwarfsToServe = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t turn, chairs;

void *dwarf(void *);
void *snowWhite(void *);

int main(int argc, const char *argv[]) {
    srand((unsigned int) time(NULL));
    sem_init(&turn, 0, 0);
    sem_init(&chairs, 0, CHAIRS);
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
    sem_destroy(&turn);
    sem_destroy(&chairs);
    free(d);
    pthread_exit(NULL);
}

void *dwarf(void *arg) {
    int id = (intptr_t) arg;
    int emptyChairs;
    while (dwarfsArrived < DWARFS) {
        usleep(rand() % 300);
        pthread_mutex_lock(&mutex);
        sem_wait(&chairs);
        sem_getvalue(&chairs, &emptyChairs);
        printf("DWARF %d has sat at the table (%d empty chairs)\n", id, emptyChairs);
        ++dwarfsArrived;
        table[in] = id;
        ++in;
        in %= CHAIRS;
        ++dwarfsToServe;
        pthread_mutex_unlock(&mutex);
        sem_post(&turn);
        usleep(rand() % 600);
        printf("DWARF %d has started eating\n", id);
        usleep(rand() % 300);
        sem_post(&chairs);
        sem_getvalue(&chairs, &emptyChairs);
        printf("DWARF %d has finished eating and left the table (%d empty chairs)\n", id, emptyChairs);
        pthread_exit(NULL);
    }
}

void *snowWhite(void *arg) {
    int dwarfsArrived = 0;
    while (dwarfsArrived < DWARFS) {
        sem_wait(&turn);
        printf("SNOW WHITE has served DWARF %d\n", table[out]);
        ++dwarfsArrived;
        dwarfsToServe--;
        ++out;
        out %= CHAIRS;
        if (!dwarfsToServe) {
            printf("SNOW WHITE has gone for a walk as there are no more dwarfs to serve at the moment\n");
        }
    }
    pthread_exit(NULL);
}