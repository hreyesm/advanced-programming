#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define TCP_PORT 8000

void handler(int signal) {
    if (signal == SIGINT) {
        printf("\nRecibí la señal Ctrl + C\n");

    } else {
        printf("\nRecibí la señal Ctrl + Z\n");
    }
    // Mandar señal a todos los sockets
}

int main(int argc, const char * argv[]) 
{
    struct sockaddr_in direccion;
    char buffer[1000];
    
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
    
    // Señales
    struct sigaction sa, sa_old;
    sa.sa_handler = handler;   // Estableciendo un gestor
    sa.sa_flags = SA_RESTART;

    // Espera la señal    
    sigaction(SIGTSTP, &sa, 0);
    sigaction(SIGTSTP, 0, &sa_old);
    // sigaction(SIGINT, &sa, 0);
    // sigaction(SIGINT, 0, &sa_old);

    // Aceptar conexiones
    while (continuar)
    {
        cliente = accept(servidor, (struct sockaddr *) &direccion, &escritos);
        
        printf("Aceptando conexiones en %s:%d \n",
               inet_ntoa(direccion.sin_addr),
               ntohs(direccion.sin_port));
        
        pid = fork();
        
        if (pid == 0) continuar = 0;
        
    }
    
    if (pid == 0) {
        
        close(servidor);
        
        if (cliente >= 0) {
            
            // Leer datos del socket
            while (leidos = read(cliente, &buffer, sizeof(buffer))) {
                write(fileno(stdout), &buffer, leidos);
                
                /* Leer de teclado y escribir en el socket */
                leidos = read(fileno(stdin), &buffer, sizeof(buffer));
                write(cliente, &buffer, leidos);
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