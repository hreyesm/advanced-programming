/*  Actividad 10: Concurrencia (Ejercicio 2)
    Curso: Programación avanzada
    Profesor: Vicente Cubells Nonell
    Autores:
        Daniela Vignau León (A01021698)
        Héctor Alexis Reyes Manrique (A01339607)
    Fecha: 29 de octubre de 2020
*/

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define SECTIONS 3
#define SECTION_WEIGHT 3
#define ROBOTS 5
#define ROBOT_WEIGHT 1
#define PRODUCT_WEIGHT 1

int mall[SECTIONS];

pthread_mutex_t mutex_in = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_out = PTHREAD_MUTEX_INITIALIZER;

void *robot(void *);

int main(int argc, const char *argv[]) {
    srand((unsigned int) time(NULL));
    int robotId = 0;     
    pthread_t *robots = (pthread_t *) malloc(sizeof(pthread_t) * ROBOTS);
    pthread_t *aux;
    for (aux = robots; aux < robots + ROBOTS; ++aux) {
        ++robotId;
        pthread_create(aux, NULL, robot, (void *) robotId);
    }
    for (aux = robots; aux < robots + ROBOTS; ++aux) {
        pthread_join(*aux, NULL);  
    }
    free(robots);
    pthread_exit(NULL);
}

void *robot(void *arg) {
    int id = (intptr_t) arg;
    int i = 0;
    int robotHasToBuy = 0;
    int rangeProductWeight = 0;
    int productWeight = 0;
    int robotWeight = ROBOT_WEIGHT;
    usleep(rand() % 300);
    while (i < SECTIONS) {
        pthread_mutex_lock(&mutex_in);
        if (mall[i] + robotWeight <= SECTION_WEIGHT) {
            robotHasToBuy = rand() % 2;
            if (robotHasToBuy) {
                mall[i] += robotWeight;
                printf("--> ROBOT %d (weight = %d) has entered SECTION %d (weight = %d/%d)\n", id, robotWeight, i, mall[i], SECTION_WEIGHT);
                if (mall[i] + PRODUCT_WEIGHT <= SECTION_WEIGHT) {
                    robotWeight += PRODUCT_WEIGHT;
                    mall[i] += PRODUCT_WEIGHT;
                    printf("ROBOT %d (weight = %d) has bought a product in SECTION %d (weight = %d/%d)\n", id, robotWeight, i, mall[i], SECTION_WEIGHT);
                    pthread_mutex_unlock(&mutex_in);
                } else {
                    printf("ROBOT %d cannot buy more products (MAX WEIGHT REACHED)\n", id);
                }
            } else {
                printf("ROBOT %d does not have to buy anything in SECTION %d\n", id, i);
            }
            usleep(rand() % 300);
            if (robotHasToBuy) {
                pthread_mutex_lock(&mutex_out);
                mall[i] -= robotWeight ;
                printf("<-- ROBOT %d (weight = %d) has left SECTION %d (weight = %d/%d)\n", id, robotWeight, i, mall[i], SECTION_WEIGHT);
                pthread_mutex_unlock(&mutex_out);
            }
            i++;
        } 
        pthread_mutex_unlock(&mutex_in);
    }
    pthread_exit(NULL);
}