#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define TCP_PORT 8000

char interruption;
int fd;

void handler(int signal) {
    if (signal == SIGINT) {
        printf("\nRecibí la señal Ctrl + C\n");
        interruption = 'Y';
    } else {
        printf("\nRecibí la señal Ctrl + Z\n");
        interruption = 'R';
    }
    write(fd, &interruption, sizeof(char));
}

int main(int argc, const char * argv[]) {
    // Señales
    struct sigaction sa, sa_old;
    sa.sa_handler = handler;   // Estableciendo un gestor
    sigaction(SIGTSTP, &sa, 0);
    sigaction(SIGINT, &sa, 0);

    struct sockaddr_in direccion;
    char buffer[100];
    
    int servidor, cliente;
    
    size_t leidos, escritos;
    int continuar = 1;
    pid_t pid;
    
    if (argc != 2) {
        printf("Use: %s IP_Servidor \n", argv[0]);
        exit(-1);
    }

    // Crear el socket
    servidor = socket(PF_INET, SOCK_STREAM, 0);
    
    // Enlace con el socket
    inet_aton(argv[1], &direccion.sin_addr);
    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;
    
    bind(servidor, (struct sockaddr *) &direccion, sizeof(direccion));
    
    // Escuhar
    listen(servidor, 10);
    
    escritos = sizeof(direccion);
    

    // Aceptar conexiones
    while (continuar) {
        cliente = accept(servidor, (struct sockaddr *) &direccion, &escritos);
        
        printf("Accepting connections in %s:%d \n", inet_ntoa(direccion.sin_addr), ntohs(direccion.sin_port));
        
        pid = fork();
        
        if (pid == 0) continuar = 0;
        
    }
    
    if (pid == 0) {
        
        close(servidor);
        
        if (cliente >= 0) {
            fd = cliente;
            // Leer datos del socket
            while (leidos = read(cliente, &buffer, sizeof(buffer))) {
                write(fileno(stdout), &buffer, leidos);
            }
        }
        
        close(cliente);
    }
    
    else if (pid > 0) {
        while (wait(NULL) != -1);
        
        // Cerrar sockets
        close(servidor);
        
    }
    return 0;
}