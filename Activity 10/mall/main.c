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
#define SECTION_WEIGHT 2
#define ROBOTS 2
#define ROBOT_WEIGHT 1
#define NO 0
#define YES 1

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
    int robotWeight = ROBOT_WEIGHT; // necessary in case the robot doesnt have to buy in the section
    usleep(rand() % 300);
    while (i < SECTIONS) {
        pthread_mutex_lock(&mutex_in);
        if (mall[i] + ROBOT_WEIGHT <= SECTION_WEIGHT) {
            mall[i] += ROBOT_WEIGHT;
            printf("--> ROBOT %d has entered SECTION %d (accumulated weight = %d)\n", id, i, mall[i]);
            robotHasToBuy = rand() % 2;
            if (robotHasToBuy == YES) {
                rangeProductWeight = SECTION_WEIGHT - mall[i]; // attempt to find the range of avaiable numbers where the product weight is and still be within the max section weight
                productWeight = rand() % SECTION_WEIGHT;
                printf("Product Weight = %d\n", productWeight);
                robotWeight += productWeight;
                mall[i] += robotWeight - ROBOT_WEIGHT; // we don't want to add twice the robot weight
                printf("--> ROBOT %d has bought a product of weight %d (accumulated weight = %d)\n", id, productWeight, mall[i]);
            } else {
                printf("ROBOT %d does not have tu purchase anything in SECTION %d\n", id, i);
            }
            pthread_mutex_unlock(&mutex_in);
            usleep(rand() % 300);
            pthread_mutex_lock(&mutex_out);
            mall[i] -= robotWeight; // reset the section weight;
            robotWeight = ROBOT_WEIGHT; // reset the robot weight;
            printf("<-- ROBOT %d has left SECTION %d (accumulated weight = %d)\n", id, i, mall[i]);
            pthread_mutex_unlock(&mutex_out);
            i++;
        } 
        pthread_mutex_unlock(&mutex_in);
    }
    pthread_exit(NULL);
}