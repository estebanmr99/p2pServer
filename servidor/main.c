#include "main.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que verifica si la lista esta vacia
int isEmpty() {
   return headL == NULL;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que retorna la cantidad de elementos en la lista
int lengthList() {
   int length = 0;
   struct node *current;
	
   for(current = headL; current != NULL; current = current->next){
      length++;
    }
   return length;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que inserta un elemento en la primera posicion de la lista
void insertFirst(int key, pthread_t thread) {

    struct node *link = (struct node*) malloc(sizeof(struct node));    // crea el link
    link->key = key;
    link->thread = thread;
	
    if(isEmpty()) {
        last = link; // conviértelo en el último link
    } else {
        headL->prev = link;
    }

    link->next = headL; // apuntar al antiguo primer link
    headL = link;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que elimina un elemento de la lista dada una llave
struct node* delete(int key) {
   struct node* current = headL;
   struct node* previous = NULL;
   if(headL == NULL) { // si la lista esta vacia
      return NULL;
   }
   while(current->key != key) { //navegar sobre la lista		
      if(current->next == NULL) {
         return NULL;
      } else {
         previous = current;
         current = current->next;             
      }
   }
   if(current == headL) { // Si encuentra la cabeza cambia el primer puntero al siguiente
      headL = headL->next;
   } else {
      current->prev->next = current->next;
   }    
   if(current == last) {
      last = current->prev;
   } else {
      current->next->prev = current->prev;
   }
   return current;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que elimina el primer elemento de la lista
struct node* deleteFirst() {
   struct node *tempLink = headL;
	
   if(headL->next == NULL){
      last = NULL;
   } else {
      headL->next->prev = NULL;
   }
	
   headL = headL->next;
   return tempLink;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que escribre bytes de manera dinamica en socket de la conexion con el cliente
int writeDataToClient(int sckt, const void *data, int datalen)
{
    const char *pdata = (const char*) data;

    while (datalen > 0){ // Si todavia quedan bytes por enviar
        int numSent = send(sckt, pdata, datalen, 0);
        if (numSent <= 0){ //Si la conexion con el cliente se cerro deja de enviar los bytes
            if (numSent == 0){
                printf("The client was not written to: disconnected\n");
            } else {
                perror("The client was not written to");
            }
            return 0;
        }
        pdata += numSent;
        datalen -= numSent;
    }
    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion de wrapper para enviar bytes al cliente
int writeStrToClient(int sckt, const char *str)
{
    return writeDataToClient(sckt, str, strlen(str));
}

//---------------------------------------------------------------------------------------------------------------------------------------
//
void responseOK(int new_socket){
    if (!writeStrToClient(new_socket, "OK") == -1){ // Responde que se proceseso el request
        close(new_socket);
        return;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
//
void responseGET(int new_socket, char *files){
    if(strlen(files) > 1){
        if (!writeStrToClient(new_socket, files) == -1){ // Responde que se proceseso el request
            close(new_socket);
            return;
        }
    } else {
        if (!writeStrToClient(new_socket, "NOTFOUND") == -1){ // Responde que se proceseso el request
            close(new_socket);
            return;
        }
    }
}

void addClientFiles(Request *request){
    if(access(INDEXFILEPATH, F_OK ) == 0){ // Si el achivo existe en la ruta de archivos
        FILE *fp = fopen(INDEXFILEPATH, "a"); // Abre archivo
        char *copyFiles = copyString(request->Files);

        char *tok = strtok(copyFiles, "\n");
        while(tok != NULL) {
            fputs(tok, fp);
            fputs(" ", fp);
            fputs(request->IP, fp);
            fputs(" ", fp);
            fputs(request->Port, fp);
            fputs("\n", fp);
            tok = strtok(NULL, "\n");
        }
        fclose(fp);
        free(copyFiles);
    }
}

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
    return result;
}


char* findFile(Request *request){
    char *result;

    if(access(INDEXFILEPATH, F_OK ) == 0){ // Si el achivo existe en la ruta de archivos
        FILE *fp = fopen(INDEXFILEPATH, "r"); // Abre archivo
        char *fileName = copyString(request->FileName);

        int lineLength = 256;
        char line[lineLength];
        const char s[2] = " ";
        char *token;

        result = (char *)calloc(1, sizeof(char));
        while(fgets(line, sizeof(line), fp)) {
            char *copyLine = copyString(line);
            token = strtok(line, s);
            if (strcmp(fileName, token) == 0){
                result = concat(result, copyLine);
            }
            free(copyLine);
        }

        fclose(fp);
        free(fileName);
    }

    return result;
}

void freeRequest(Request *request){
    if (request->Method == GET){
        free(request->FileName);
    } else if (request->Method == ONLINE){
        free(request->IP);
        free(request->Port);
        free(request->Files);
    } else if (request->Method == OFFLINE){
        free(request->IP);
        free(request->Port);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que realiza la conexion mediante un hilo, por lo cual debe ser un puntero
void *threaded(void *args)
{
    char buffer[30000];
    int socket = *((int*)args);
    free(args);
    
    read(socket, buffer, 30000);

    // char requestString[300] = "ONLINE 192.168.100.1 8080\nNOMBREARCHIVO.png HASH TAMANO AUTOR\nNOMBREARCHIVO.jpe HASH TAMANO AUTOR";
    // char requestString[300] = "GET NOMBREARCHIVO.png";
    printf("%s",buffer);
    Request request = Request_constructor(buffer);
    // Request request = Request_constructor(requestString);

    if (request.Method == ONLINE){
        addClientFiles(&request);
        responseOK(socket);

    } else if (request.Method == GET){
        char* files = findFile(&request);
        responseGET(socket, files);
        free(files);
        
    } else if (request.Method == OFFLINE){

    }
    freeRequest(&request);
    close(socket);
    pthread_mutex_lock(&listMutex);
    free(delete(pthread_self()));
    pthread_mutex_unlock(&listMutex);
    pthread_exit(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que realiza el launch del servidor y elegir el tipo de servidor
void launch(Server *server)
{
    while(killFlag){
        printf("\n===== WAITING FOR CONNECTION =====\n");

        int address_length = sizeof(server->address);

        int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length);

        pthread_t t; //creacion de un hilo para cada request
        int *socket = malloc(sizeof(int)); //se asigna un socket
        *socket = new_socket;
        pthread_create(&t,NULL,threaded,socket); //se ejecuta el request del hilo al servidor
        insertFirst(t, t); //se guarda el id del hilo en la lista enlazada
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que encuentra la IP de la interfaz e inicia el servidor
void *serverFunc(void *args) {
    IPFinder ipObj = finder_constructor();
    printf("IP: %s\n", ipObj.ip);
    printf("Port: %d\n", PORT);

    Server server = server_constructor(AF_INET,SOCK_STREAM, 0, ipObj.ip, PORT, 5000, launch);
    server.launch(&server);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que mata todos los hilos en caso de que haya que un mensaje de kill
void killThreads(){
    int lengthOfList = lengthList();
    if (lengthOfList > 0)
    {
        for(int i = 0; i < lengthOfList; i++){
            pthread_mutex_lock(&listMutex);
            node *threadToDelete = deleteFirst();
            pthread_mutex_unlock(&listMutex);
            pthread_cancel(threadToDelete->thread);
            free(threadToDelete);
        }
    }
    pthread_cancel(serverThread);//Pthread_cancel
    pthread_exit(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion main
int main(int argc, char**argv)
{
    int N;
    int T = 0;
    int P = 0;
    char consoleInput[30];

    killFlag = 1;

    FILE *fp = fopen(INDEXFILEPATH, "w"); // Abre archivo
    fclose(fp);

    pthread_create(&serverThread,NULL,serverFunc,NULL); // Crea el hilo donde esta funcionando todo

    while(killFlag){
        scanf("%s", consoleInput);
        if (strcmp(consoleInput, "kill") == 0){ // Si hay un mensaje por consola que se kill mata el servidor
            killFlag = 0;
        }
        strcpy(consoleInput,"");
    }

    close(server.socket);
    killThreads();

    return 0;
}
