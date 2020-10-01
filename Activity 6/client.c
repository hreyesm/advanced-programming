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

int fd;

void alarmHandler(int signal) {
    kill(nextPid, SIGUSR1);
    currState = 'R';
    printf("State: %c\n", currState);
    write(fd, &currState, sizeof(char));
}

void handler(int signal) {
    struct sigaction sa;
    sa.sa_handler = alarmHandler;       
    currState = 'G';
    printf("State: %c\n", currState);
    write(fd, &currState, sizeof(char));
    alarm(8);
    sigaction(SIGALRM, &sa, 0);
}

int main(int argc, char * argv[]) {    
    struct sigaction sa;
    sa.sa_handler = handler;
    sigaction(SIGUSR1, &sa, 0);
    // sigset_t blockMask; 
    // sigfillset(&blockMask);
    // sigdelset(&blockMask, SIGUSR1);
    // sigdelset(&blockMask, SIGALRM);
    // sa.sa_mask = blockMask;

    struct sockaddr_in address;
    char buffer, receivedSignal = '\0', *value;
    int arg, flag = 0, isFirst, semaphore;
    ssize_t receivedData, sentData;

    while ((arg = getopt(argc, argv, "f")) != -1) {
        switch (arg) {
            case 'f':
                isFirst = 1;
                break;
            default:
                abort();
        }
    }

    semaphore = socket(PF_INET, SOCK_STREAM, 0);
    fd = semaphore;

    if (isFirst == 1) {
        inet_aton(argv[2], &address.sin_addr);
    } else {
        inet_aton(argv[1], &address.sin_addr);
    }

    address.sin_port = htons(TCP_PORT);
    address.sin_family = AF_INET;
    sentData = connect(semaphore, (struct sockaddr *) &address, sizeof(address));

    if (sentData == 0)  {
        printf("PID: %d\nEnter the PID of the semaphore on the right: ", getpid());
        scanf("%d", &nextPid);
        printf("Sending signals to sempahore with PID %d\n", nextPid);
        
        if (isFirst == 1) {
            printf("State: %c\n", currState);
            kill(nextPid, SIGUSR1);
            isFirst = 0;
        }

        while ((receivedData = read(semaphore, &buffer, sizeof(buffer)))) {
            if (receivedData == 1) { 
                if (receivedSignal != buffer) {
                    receivedSignal = buffer;
                    if (flag == 0) {
                        lastState = currState;
                        flag = 1;
                    }
                    currState = receivedSignal;
                    printf("State: %c\n", currState);
                    write(fd, &currState, sizeof(char));
                    alarm(0);
                } else {
                    receivedSignal = '\0';
                    currState = lastState;
                    flag = 0;
                    if (currState == 'G') {
                        printf("\nRestarting semaphores...\n\n");
                        raise(SIGUSR1);
                    }
                }
            }
        }
    }

    close(semaphore);
    
    return 0;
}