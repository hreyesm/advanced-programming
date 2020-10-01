#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>

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
    char buffer;
    int cliente;
    ssize_t leidos, escritos;
    int arg;
    char *value;
    int firstSignal;
    char receivedSignal = 'X';


    // Validate argument
    while ((arg = getopt(argc, argv, "f")) != -1)
    switch (arg) {
        case 'f':
            firstSignal = 1;
            break;
        default:
            abort();
    }

    // Socket
    cliente = socket(PF_INET, SOCK_STREAM, 0);

    if (firstSignal == 1) {
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
        
        if (firstSignal == 1) {
            printf("Sending the first signal\n");
            kill(nextPid, SIGUSR1);
            firstSignal = 0;
        }

        // Escribir datos en el socket
        while (leidos = read(cliente, &buffer, sizeof(buffer))) {
            if (leidos == 1) { 
                printf("Leidos: %d\n", leidos);
                if (receivedSignal != buffer) { // Stop semaphores, a new signal has been received
                    printf("Buffer: %c\n", buffer);
                    receivedSignal = buffer;
                    printf("Recibí la señal %c\n", receivedSignal);
                    lastState = currState;
                    currState = receivedSignal;
                    printf("State: %c\n", currState);
                    alarm(0);
                } else { // The same signal has been received, restart semaphores
                    currState = lastState;
                    receivedSignal = 'X';
                    if (currState == 'G') {
                        printf("Restarting...\n");
                        raise(SIGUSR1);
                    }
                }
            }
        }
    }
    
    // Cerrar sockets
    close(cliente);
    
    return 0;
}