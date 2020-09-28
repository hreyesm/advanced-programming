#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define TCP_PORT 8000

int main(int argc, const char * argv[]) {
    struct sockaddr_in address;
    char buffer[1000];
    
    int server, client;
    
    int leidos, escritos;
    
    if (argc != 2) {
        printf("Error. Use: %s A.B.C.D \n", argv[0]);
        exit(-1);
    }
    
    //Crear el socket
    server = socket(PF_INET, SOCK_STREAM, 0);
    
    // Enlace con el socket
    address.sin_port = htons(TCP_PORT);
    address.sin_family = AF_INET;
    inet_aton(argv[1], &address.sin_addr);
    
    bind(server, (struct sockaddr *) &address, sizeof(address));
    
    // Escuchar a los clients
    listen(server, 10);
    
    // Comunicación
    socklen_t tamano = sizeof(address);
    
    client = accept(server, (struct sockaddr *) &address, &tamano);

    if (client >= 0) {
        printf("Aceptando conexiones en %s:%d \n",
               inet_ntoa(address.sin_addr),
               ntohs(address.sin_port));
        
        // Leer de socket y escribir en pantalla
        while (leidos = read(client, &buffer, sizeof(buffer))) {
            write(fileno(stdout), &buffer, leidos);
            
            leidos = read(fileno(stdin), &buffer, sizeof(buffer));
            write(client, &buffer, leidos);
        }     
    }

    // Cerrar el socket
    
    close(server);
    close(client);
    
    return 0;
}