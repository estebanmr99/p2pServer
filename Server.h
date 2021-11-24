#ifndef Server_h

#define Server_h
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>

// Struct para manejar el servidor, la funcion de lauch, el socket, y el puerto desde la funcion de main
typedef struct Server
{
    int domain;
    int service;
    int protocol;
    char *interface;
    int port;
    int backlog;
    int socket;
    struct sockaddr_in address;    
    void (*launch)(struct Server *server);
}Server;

// Declara un puntero a la función que toma un argumento de tipo void * y devuelve un struct del servidor
Server server_constructor(int domain, int service, int protocol, char *interface, int port, int backlog, void (*launch)(struct Server *server));

#endif