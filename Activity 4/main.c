/*  Actividad individual: Procesos
    Curso: Programación avanzada
    Profesor: Vicente Cubells Nonell
    Autor: Héctor Alexis Reyes Manrique (A01339607)
    Fecha: 10-9-2020
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/wait.h>

#define H 30 // Max characters in histogram

typedef struct {
    int id;
    int average;
    char *histogram;
} process;

void printProcess(process *p) {
    printf("%20d%10d%35s\n", p->id, p->average, p->histogram);
}

int max = 0;
int main(int argc, char *argv[]) {
    int arg, average, n;
    char *value;
    process *processes = (process *) malloc(sizeof(process) * n);
    process *p = processes;

    // Validate argument
    while ((arg = getopt(argc, argv, "n:")) != -1)
        switch (arg) {
            case 'n':
                value = optarg;
                if (isdigit(*value)) {
                    n = atoi(value);
                } else {
                    printf("Argument is not a number.");
                    return 1;
                }
                break;
            case '?':
                return 1;
            default:
                abort();
        }
    

    // Create processes
    int i = 0;
    pid_t pid;
    for (i = 0; i < n; i++) {
        pid = fork();
        if (pid == -1) {
            printf("An error occurred while creating a child.\nProcesses created so far: %d", i + 1);
            exit(-1);
        } else if (pid == 0) {
            average = (getpid() + getppid()) / 2;
            printf("\n%d\tChild PID = %d, Parent PID = %d\n\tAverage of parent and child PIDs = %d\n", i, getpid(), getppid(), average);
            exit(average);
        } else {
            if (waitpid(pid, &average, 0) != -1) {
                if (WIFEXITED(average)) {
                    (processes + i)->id = pid;
                    (processes + i)->average = WEXITSTATUS(average);
                    if (WEXITSTATUS(average) > max) 
                        max = WEXITSTATUS(average);
                }
            }
        }
    }

    // Generate histograms
    int chars = 0;
    char *c;
    for (; p < processes + n; p++) {
        chars = p->average * H / max;
        p->histogram = (char *) malloc(sizeof(char) * chars);
        strcpy(p->histogram, "#");
        c = p->histogram + 1;
        for (; c < p->histogram + chars; c++) {
            strcpy(c, "#");
        }
    }

    // Print table
    printf("\n%20s%10s%35s\n", "Child PID", "Average", "Histogram");
    printf("%20s%10s%35s\n", "---------", "-------", "---------");
    p = processes;
    for (; p < processes + n; p++) {
        printProcess(p);
    }

    // Free processes
    p = processes;
    for (; p < processes + n; p++) {
        free(p->histogram);
    }
    free(processes);
    return 0;
}