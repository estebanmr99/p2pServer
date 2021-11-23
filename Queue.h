#ifndef Queue_h
#define Queue_h

// Struct de un nodo para la cola (encola las solicitudes del cliente)
typedef struct Node
{
    struct Node *next;
    int *client_socket;
}Node;

// Metodos para la cola 
void enqueue(int *client);
int *dequeue();

#endif
