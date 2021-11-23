#ifndef MAIN_h

#include <stdio.h>
#include "Server.h"
#include <string.h>
#include <unistd.h>
#include "HTTPRequest.h"
#include "IPFinder.h"
#include <pthread.h>
#include "Queue.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8000
#define PARAMETERKEY "/?file=" // Parametro que tiene que enviar el cliente para indicar el archivo a solicitar
#define FOLDERPATH "./files/" // Ruta donde se encuentran los archivos disponibles del servidor
#define NOTFOUNDPAGEPATH "./files/404.html" // Archivo de 404 cuando la solicitud falla

// Struct para guardar la informacion del archivo que se abrio dado X request
// Guarda el tamaño del archivo y el contenido del archivo
typedef struct OpenedFile
{
    long fsize;
    char * msg;
} OpenedFile;

// Struct para agrupar la informacion del tipo de modo del servidor y el socket
typedef struct Args
{
    ServerType type;
    int socket;

} Args;


// Struct para las lista enlazada doble para mantener trackeo de los hilos o procesos creados
typedef struct node {
   pthread_t thread;
   pid_t pid;
   int key;
   struct node *next;
   struct node *prev;
} node;


pthread_t *pool;
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pool_cond = PTHREAD_COND_INITIALIZER;
pthread_t serverThread;
ServerType serverType;
Server server;
int killFlag;
struct node *headL = NULL; // Puntero a la cabeza de la lista
struct node *last = NULL; // Puntero a la cola de la lista
struct node *current = NULL;

void preforked(int nclients,Server *server);

#endif