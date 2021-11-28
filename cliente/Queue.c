#include "Queue.h"
#include <stdlib.h>

Node* head = NULL;
Node* tail = NULL;

// Funcion para enconlar las solicitudes de los clientes
void enqueue(int *client){
    Node *node = malloc(sizeof(Node));
    node->client_socket = client;
    node->next = NULL;

    if(tail==NULL)
        head = node;
    else
        tail->next = node;
    tail = node;
}

// Funcion para desenconlar las solicitudes de los clientes
int *dequeue(){
    if(head == NULL) // Si ya no hay nada en la cola retorna null
        return NULL;
    else{
        Node *temp = head;
        int *result = head->client_socket;
        head = head->next;
        if(head == NULL)
            tail = NULL;
        free(temp);
        return result;
    }
}