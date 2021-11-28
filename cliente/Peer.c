#include "Peer.h"


Client client_constructor(int domain, int service, int protocol, char *interface, int port, int backlog, void (*launch)(Client *client)){
    Client client;

    client.domain = domain;
    client.service = service;
    client.protocol = protocol;
    client.interface = interface;
    client.port = port;
    client.backlog = backlog;

    client.address.sin_family = domain;
    client.address.sin_port = htons(port);     // La función htons se puede utilizar para convertir un número de puerto IP en el host
                                               // orden de bytes al número de puerto IP en orden de bytes de red.

    client.address.sin_addr.s_addr = inet_addr(interface);     // La función htonl se puede utilizar para convertir una dirección IPv4 en el host
                                                               // orden de bytes a la dirección IPv4 en orden de bytes de red.

    client.socket = socket(domain,service,protocol); // Devuelve el descriptor del archivo de socket 
    if(client.socket == 0){
        perror("Failed to connect socket....\n");
        exit(1);
    }

    int reuse;
    if (setsockopt(client.socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1) // Si el socket no se esta usando lo usa
        printf("Reuse port Error : %s\n", strerror(errno));

    if((bind(client.socket, (struct sockaddr *)&client.address,sizeof(client.address))) < 0){
        perror("Failed to bind socket....\n");
        exit(1);
    }

    if(listen(client.socket,client.backlog) < 0){
        perror("Failed to start listening....\n");
        exit(1);
    }

    client.launch = launch;

    return client;
}




