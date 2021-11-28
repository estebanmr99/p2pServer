#ifndef Peer_h

#define Peer_h
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>


#define PORT 8082
#define CHUNK_SIZE 1024
#define IP "127.0.0.1"
#define SERVER_IP "192.168.0.22"
#define SERVER_PORT 8001


typedef struct file_info{
    char* file_name;
    uint32_t file_length;
}file_info;

typedef struct peer{
    struct in_addr address;
    uint16_t port;
}peer;

typedef struct chunk{
    uint32_t index;
    int is_possessed;
//    std::vector<peer> peers;
}chunk;

// Struct para manejar el servidor, la funcion de lauch, el socket, y el puerto desde la funcion de main
typedef struct Client
{
    int domain;
    int service;
    int protocol;
    char *interface;
    int port;
    int backlog;
    int socket;
    struct sockaddr_in address;    
    void (*launch)(struct Client *client);
}Client;

// Declara un puntero a la función que toma un argumento de tipo void * y devuelve un struct del servidor
Client client_constructor(int domain, int service, int protocol, char *interface, int port, int backlog, void (*launch)(struct Client *client));

#endif