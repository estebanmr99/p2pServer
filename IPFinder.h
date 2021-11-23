#ifndef IPFinder_h

#include<stdio.h>	//printf
#include<string.h>	//memset
#include<errno.h>	//errno
#include<sys/socket.h>
#include<netdb.h>
#include<ifaddrs.h>
#include<stdlib.h>
#include<unistd.h>

// Struct para almancer la direccion IP disponible en la interdaz de red
typedef struct IPFinder
{
    char *ip;
} IPFinder;


// Constructor para crear un objeto de IPFinder 
IPFinder finder_constructor();

#endif