/*
    Preguntas
    1. ¿Cómo vamos a dar a conocer el PID del semáforo para enviar señales?
        - Enviada por los sockets, se almacena en el semáforo
        - Topología estrella inicialmente, y después es token ring
    2. Al leer la señal Ctrl + C en el servidor, se hace un broadcast automático en todos los hijos?
    3. Manejador...
        - ¿Cómo recibimos las señales del server en los clientes? Manejados como un string? 
    4. Todas las cosas enviadas por los sockets, son strings...? 
*/


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

void handler(int signal) {
    if (signal == SIGUSR1) {
        printf("Estado: Verde\n");
        sleep(5);
        kill(nextPid, SIGUSR1);
        printf("Estado: Rojo\n");
    }
}

int main(int argc, char * argv[]) {
    // Señales
    struct sigaction sa, sa_old;
    sa.sa_handler = handler;   // Estableciendo un gestor        
    sigaction(SIGUSR1, &sa, 0);

    struct sockaddr_in direccion;
    char buffer[1000];
    int cliente;
    ssize_t leidos, escritos;
    int arg;
    char *value;
    int firstFlag;

    // if (argc != 2) {
    //     printf("Use: %s IP_Servidor \n", argv[0]);
    //     exit(-1);
    // }

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
        inet_aton(argv[2], &direccion.sin_addr);
    } else {
        inet_aton(argv[1], &direccion.sin_addr);
    }

    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;
    escritos = connect(cliente, (struct sockaddr *) &direccion, sizeof(direccion));

    // Communication
    if (escritos == 0)  {
        printf("Conectado a %s:%d \n", inet_ntoa(direccion.sin_addr), ntohs(direccion.sin_port));
        printf("My PID is %d\nEnter the PID of the semaphore on the right: ", getpid());
        scanf("%d", &nextPid);
        printf("Sending signals to sempahore with PID %d\n", nextPid);
        if (firstFlag == 1) {
            printf("Sending the first signal\n");
            kill(nextPid, SIGUSR1);
            firstFlag = 0;
        }

        // sigaction(SIGUSR1, 0, &sa_old);

        // Escribir datos en el socket
        while ((leidos = read(fileno(stdin), &buffer, sizeof(buffer)))) {
            write(cliente, &buffer, leidos);
            
            /* Lee del buffer y escribe en pantalla */
            leidos = read(cliente, &buffer, sizeof(buffer) );
            write(fileno(stdout), &buffer, leidos);
        }
    }
    
    // Cerrar sockets
    close(cliente);
    
    return 0;
}