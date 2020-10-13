/*  Actividad: IPC
    --------------
    Curso:    Programación avanzada
    Profesor: Vicente Cubells Nonell
    Autor:    Héctor Alexis Reyes Manrique (A01339607)
              (en colaboración con Daniela Vignau León)
    Fecha:    17-9-2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

typedef struct {
    int fd[2];
} pipe_t;


int main(int argc, char *argv[]) {
    int arg, i, j, n = 0;
    char message = 'T';
    char *value;
    char m;
    pid_t pid;

    // Validate argument
    while ((arg = getopt(argc, argv, "n:")) != -1) {
        switch (arg) {
            case 'n':
                value = optarg;
                if (isdigit(*value)) {
                    n = atoi(value);
                    if (n < 2) {
                        fprintf(stderr, "option requires a number >= 2 as an argument\n");
                        return 1;
                    }
                } else {
                    fprintf(stderr, "option requires a number as an argument\n");
                    return 1;
                }
                break;
            case '?':
                return 1;
            default:
                abort();
        }
    }

    if (argc != 3) {
        printf("incorrect usage");
        return 1;
    }
    
    pipe_t *pipes = (pipe_t *) malloc(sizeof(pipe_t) * n);
    pipe_t *p = pipes;
    
    for(; p < pipes + n; p++) {
        pipe(p->fd);
    }

    for (i = 0; i < n - 1; i++) {
        pid = fork();
        switch (pid) {
            case -1:
                fprintf(stderr, "error while creating a child");
            case 0:
                while (1) {
                    for(j = 0; j < n - 1; j++) {
                        if((pipes + i)->fd[READ] != (pipes + j)->fd[READ]) {
                            close((pipes + j)->fd[READ]);
                        }
                    }
                    close((pipes + i)->fd[WRITE]);
                    read((pipes + i)->fd[READ], &m, sizeof(char));
                    printf("--> Soy el proceso con PID %d y recibí el testigo %c, el cual tendré por 5 segundos\n", getpid(), m);
                    sleep(5);
                    close((pipes + i + 1)->fd[READ]);
                    printf("<-- Soy el proceso con PID %d y acabo de enviar el testigo %c\n\n", getpid(), m);
                    write((pipes + i + 1)->fd[WRITE], &m, sizeof(char));
                }
        } 
    }

    printf("<-- Soy el proceso padre con PID %d y acabo de enviar el testigo %c\n\n", getpid(), message);
    write(pipes->fd[WRITE], &message, sizeof(char));
    
    while (1) {
        close((pipes + 1)->fd[WRITE]);
        read((pipes + n - 1)->fd[READ], &m, sizeof(char));
        printf("--> Soy el proceso padre con PID %d y recibí el testigo %c, el cual tendré por 5 segundos\n", getpid(), m);
        sleep(5);
        close(pipes->fd[READ]);
        printf("<-- Soy el proceso padre con PID %d y acabo de enviar el testigo %c\n\n", getpid(), m);
        write(pipes->fd[WRITE], &m, sizeof(char));
    }

    return 0;
}