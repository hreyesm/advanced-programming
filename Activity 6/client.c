#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#define TCP_PORT 8000

int nextPid;
char currState = 'R';
char lastState;

void alarmHandler(int signal) {
    kill(nextPid, SIGUSR1);
    currState = 'R';
    printf("State: %c\n", currState);
}

void handler(int signal) {
    struct sigaction sa;
    sa.sa_handler = alarmHandler;       
    // printf("State: %c\n", currState);
    if (signal == SIGUSR1) {
        currState = 'G';
        printf("\nState: %c\n", currState);
        alarm(5);
        sigaction(SIGALRM, &sa, 0);
    }
}

int main(int argc, char * argv[]) {
    // Signals
    struct sigaction sa, sa_old;
    sa.sa_handler = handler;       
    sigaction(SIGUSR1, &sa, 0);

    struct sockaddr_in address;
    char buffer[100];
    int cliente;
    ssize_t leidos, escritos;
    int arg;
    char *value;
    int firstFlag;

    // Validate argument
    while ((arg = getopt(argc, argv, "f")) != -1)
    switch (arg) {
        case 'f':
            firstFlag = 1;
            break;
        default:
            abort();
    }

    // Socket
    cliente = socket(PF_INET, SOCK_STREAM, 0);

    if (firstFlag == 1) {
        inet_aton(argv[2], &address.sin_addr);
    } else {
        inet_aton(argv[1], &address.sin_addr);
    }

    address.sin_port = htons(TCP_PORT);
    address.sin_family = AF_INET;
    escritos = connect(cliente, (struct sockaddr *) &address, sizeof(address));

    // Communication
    if (escritos == 0)  {
        printf("Connected to %s:%d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        
        printf("My PID is %d\nEnter the PID of the semaphore on the right: ", getpid());
        scanf("%d", &nextPid);
        printf("Sending signals to sempahore with PID %d\n", nextPid);
        
        if (firstFlag == 1) {
            printf("Sending the first signal\n");
            kill(nextPid, SIGUSR1);
            firstFlag = 0;
        }

        // Escribir datos en el socket
        while (leidos = read(cliente, &buffer, sizeof(buffer))) {
            /* Lee del buffer y escribe en pantalla */
            if(leidos == 1) {
                printf("Recibí la señal %s\n", buffer);
                lastState = currState;
                currState = *buffer;
                printf("STATE - %c\n", currState);
                alarm(5);
                // if(lastState == 'G') {
                //     raise(SIGUSR1);
                //     printf("Restarting...\n");
                //     currState = lastState;
                //     printf("State: %c\n", currState);
                // }
                // printf("Ya no estoy en el handler!!");
            }
        }
    }
    
    // Cerrar sockets
    close(cliente);
    
    return 0;
}
/*
    - Los semáforos tienen su funcionamiento normal con alarm()
    - En cuanto se recibe un ctrl C, ambos semáforos lo reconocen
    - De la linea 93 a la 96, quería hacer que al volver a presionar Ctrl C en el servidor,
        que el que tiene como lastState Green, que se mandara una señal a si mismo para que volviera empezar
    - Hay que ver la manera de guardar el buffer y que si es el mismo, entonces se levanta la señal
*/