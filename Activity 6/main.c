#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define TCP_PORT 8000

char interruption;
int fd;
int semaphoreId = 0;

void handler(int signal) {
    if (signal == SIGINT) {
        interruption = 'I';
    } else if (signal == SIGTSTP) {
        interruption = 'R';
    }
    write(fd, &interruption, sizeof(char));
}

int main(int argc, const char * argv[]) {    
    struct sigaction sa;
    sa.sa_handler = handler;
    sigaction(SIGTSTP, &sa, 0);
    sigaction(SIGINT, &sa, 0);
    // sigset_t blockMask; 
    // sigfillset(&blockMask);
    // sigdelset(&blockMask, SIGINT);
    // sigdelset(&blockMask, SIGTSTP);
    // sa.sa_mask = blockMask;

    struct sockaddr_in address;
    char buffer;
    int connect = 1, console, semaphore; 
    size_t receivedData, sentData;
    pid_t pid;
    
    console = socket(PF_INET, SOCK_STREAM, 0);
    inet_aton(argv[1], &address.sin_addr);
    address.sin_port = htons(TCP_PORT);
    address.sin_family = AF_INET;
    
    bind(console, (struct sockaddr *) &address, sizeof(address));
    listen(console, 10);
    sentData = sizeof(address);

    while (connect) {
        semaphore = accept(console, (struct sockaddr *) &address, &sentData);
        semaphoreId += 1;
        pid = fork();
        if (pid == 0) {
            connect = 0;
        }
    }

    if (pid == 0) {
        close(console);
        if (semaphore >= 0) {
            fd = semaphore;
            while ((receivedData = read(semaphore, &buffer, sizeof(buffer)))) {
                if (receivedData == 1) {
                    printf("Semaphore %d changed state to ", semaphoreId);
                    switch (buffer) {
                        case 'R':
                            printf("RED\n");
                            break;
                        case 'G':
                            printf("GREEN\n");
                            break;
                        case 'I':
                            printf("INTERMITTENT\n");
                            break;
                    }
                }
                buffer = '\0';
            }
        }
        close(semaphore);
    } else if (pid > 0) {
        while (wait(NULL) != -1);
        close(console);
    }
    
    return 0;
}