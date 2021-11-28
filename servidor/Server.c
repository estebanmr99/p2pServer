#include "Server.h"

// Declara un puntero a la función que toma un argumento de tipo void * y devuelve un struct del servidor
Server server_constructor(int domain, int service, int protocol, char *interface, int port, int backlog, void (*launch)(Server *server)){
    Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);     // La función htons se puede utilizar para convertir un número de puerto IP en el host
                                               // orden de bytes al número de puerto IP en orden de bytes de red.

    server.address.sin_addr.s_addr = inet_addr(interface);     // La función htonl se puede utilizar para convertir una dirección IPv4 en el host
                                                               // orden de bytes a la dirección IPv4 en orden de bytes de red.

    server.socket = socket(domain,service,protocol); // Devuelve el descriptor del archivo de socket 
    if(server.socket == 0){
        perror("Failed to connect socket....\n");
        exit(1);
    }

    int reuse;
    if (setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1) // Si el socket no se esta usando lo usa
        printf("Reuse port Error : %s\n", strerror(errno));

    if((bind(server.socket, (struct sockaddr *)&server.address,sizeof(server.address))) < 0){
        perror("Failed to bind socket....\n");
        exit(1);
    }

    if(listen(server.socket,server.backlog) < 0){
        perror("Failed to start listening....\n");
        exit(1);
    }

    server.launch = launch;

    return server;
}