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
void insertFirst(int key, pthread_t thread, pid_t pid) {

    struct node *link = (struct node*) malloc(sizeof(struct node));    // crea el link
    link->key = key;
    link->thread = thread;
    link->pid = pid;
	
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
// Funcion que abre un archivo dado una direccion en disco y retorna el contenido del archivo con su tamano
OpenedFile openFile(char *filePath){
    OpenedFile file;
    long fsize;
    FILE *fp = fopen(filePath, "rb"); // Abre archivo
    if (!fp){
        perror("The file was not opened");    
        exit(1);
    }

    if (fseek(fp, 0, SEEK_END) == -1){ // Verifica que el archivo exista
        perror("The file was not seeked");
        exit(1);
    }

    fsize = ftell(fp);
    if (fsize == -1) {
        perror("The file size was not retrieved");
        exit(1);
    }
    rewind(fp);

    char *msg = (char*) malloc(fsize);
    if (!msg){
        perror("The file buffer was not allocated\n");
        exit(1);
    }

    if (fread(msg, fsize, 1, fp) != 1){ // Lee el archivo
        perror("The file was not read\n");
        exit(1);
    }
    fclose(fp); // Cierra el archivo

    printf("The file size is %ld\n", fsize);

    file.fsize = fsize;
    file.msg = msg;
    return file;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que dependiendo del archivo solicitado genera el mimetype del archivo para los headers del response
char *generateMimeType(const char *extension){
    char *mimeType;

    if(strcmp(extension, ".mp3") == 0){
        mimeType = malloc(sizeof(char) * (strlen("audio/mpeg") + 1));
        strcpy(mimeType, "audio/mpeg");
    } else if(strcmp(extension, ".csv") == 0){
        mimeType = malloc(sizeof(char) * (strlen("text/csv") + 1));
        strcpy(mimeType, "text/csv");
    } else if(strcmp(extension, ".mp4") == 0){
        mimeType = malloc(sizeof(char) * (strlen("video/mp4") + 1));
        strcpy(mimeType, "video/mp4");
    } else if(strcmp(extension, ".htm") == 0 || strcmp(extension, ".html") == 0){
        mimeType = malloc(sizeof(char) * (strlen("text/html") + 1));
        strcpy(mimeType, "text/html");
    } else if(strcmp(extension, ".jpeg") == 0 || strcmp(extension, ".jpg") == 0){
        mimeType = malloc(sizeof(char) * (strlen("image/jpeg") + 1));
        strcpy(mimeType, "image/jpeg");
    } else if(strcmp(extension, ".png") == 0){
        mimeType = malloc(sizeof(char) * (strlen("image/png") + 1));
        strcpy(mimeType, "image/png");
    } else if(strcmp(extension, ".pdf") == 0){
        mimeType = malloc(sizeof(char) * (strlen("application/pdf") + 1));
        strcpy(mimeType, "application/pdf");
    } else if(strcmp(extension, ".rar") == 0){
        mimeType = malloc(sizeof(char) * (strlen("application/vnd.rar") + 1));
        strcpy(mimeType, "application/vnd.rar");
    } else if(strcmp(extension, ".tar") == 0){
        mimeType = malloc(sizeof(char) * (strlen("application/x-tar") + 1));
        strcpy(mimeType, "application/x-tar");
    } else if(strcmp(extension, ".txt") == 0){
        mimeType = malloc(sizeof(char) * (strlen("text/plain") + 1));
        strcpy(mimeType, "text/plain");
    } else if(strcmp(extension, ".css") == 0){
        mimeType = malloc(sizeof(char) * (strlen("text/csv") + 1));
        strcpy(mimeType, "text/css");
    }

    return mimeType;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que obtiene la extension de un archivo para determinar el mimetype
const char *getFilenameExt(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que hace el formato del response de un request GET para enviarlo por el socket
void responseGet(int new_socket, char *fileName, char *path){
    OpenedFile file;
    char clen[40];
    char ctype[40];
    char cdisposition[80];

    printf("Requested file name: %s\n", fileName);
    int newSize = strlen(path)  + strlen(fileName) + 1;     // Determina el tamano del puntero de la ruta del archivo
    char * filePathConcat = (char *)malloc(newSize);    // Aparta el tamano del buffer

    // hace la copia y concatena la ruta de archivos + el nombre del archivo
    strcpy(filePathConcat, path);
    strcat(filePathConcat, fileName);

    if(access(filePathConcat, F_OK ) == 0){ // Si el achivo existe en la ruta de archivos
        file = openFile(filePathConcat);

        if (!writeStrToClient(new_socket, "HTTP/1.1 200 OK\r\n")){ // Escribe el header del HTTP status
            close(new_socket);
            return;
        }

        sprintf(clen, "Content-length: %ld\r\n", file.fsize);
        if (!writeStrToClient(new_socket, clen)){ // Escribe el header de content length
            close(new_socket);
            return;
        }

        sprintf(cdisposition, "Content-Disposition: attachment; filename=\"%s\"\r\n", fileName); // Concatena el nombre del archivo al header

        const char *fileExtension = getFilenameExt(fileName);
        char *contentType = generateMimeType(fileExtension);
        sprintf(ctype, "Content-Type: %s\r\n", contentType);

        if (!writeStrToClient(new_socket, cdisposition)){ // Escribe el header de content Disposition
            close(new_socket);
            return;
        }

        if (!writeStrToClient(new_socket, ctype)){  // Escribe el header de content type
            close(new_socket);
            return;
        }

        free(contentType);
    } else {
        file = openFile(NOTFOUNDPAGEPATH); // En caso de que no exista el archivo se volvera un 404 con un archivo html ya definido

        if (!writeStrToClient(new_socket, "HTTP/1.1 404 Not Found\r\n")){ // Escribe el header del HTTP status
            close(new_socket);
            return;
        }

        sprintf(clen, "Content-length: %ld\r\n", file.fsize);
        if (!writeStrToClient(new_socket, clen)){ // Escribe el header de content length
            close(new_socket);
            return;
        }

        if (!writeStrToClient(new_socket, "Content-Type: text/html\r\n")){  // Escribe el header de content type
            close(new_socket);
            return;
        }
    }

    if (!writeStrToClient(new_socket, "Connection: close\r\n\r\n") == -1){ // Escribre que la conexion ya se cerro
        close(new_socket);
        return;
    }

    if (!writeDataToClient(new_socket, file.msg, file.fsize)){ // Escribe el contenido del archivo a devolver, ya sea el solicitado el 404
        close(new_socket);
        return;
    }

    printf("The file was sent successfully\n");
    free(file.msg);
    free(filePathConcat);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que hace el formato del response de un request POST para enviarlo por el socket
void responsePost(){

}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que realiza la conexion mediante FIFO
void FIFO(int socket)
{
    char buffer[30000];

    read(socket, buffer, 30000); //lee el descriptor del socket

    HTTPRequest request = HTTPRequest_constructor(buffer); //Construye la estructura del request GET
    
    if (request.Method == GET || request.Method == POST){

        if (request.Method == GET){
            char *copyRequestURI = malloc(sizeof(char) * (strlen(request.URI) + 1));
            strcpy(copyRequestURI, request.URI);

            char *parameter = malloc(sizeof(char) * (strlen(request.URI) + 1));
    
            strncpy(parameter, request.URI, 7);

            if(strcmp(parameter, PARAMETERKEY) == 0){
                char *fileName = malloc(sizeof(char) * (strlen(request.URI) + 1));
                strncpy(fileName, request.URI+7, strlen(request.URI));

                responseGet(socket,fileName,FOLDERPATH);   
                free(fileName);                     
            }
            free(copyRequestURI);
            free(parameter);

        }else if (request.Method == POST){
            responsePost();                 //Codigo para POST
        }
    }
    close(socket);
    strcpy(buffer,"");
}



//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que realiza la conexion al servidor FORKED
void forked(int socket,int pid) 
{
    FIFO(socket); //se llama a la funcion FIFO porque realiza una unica conexion
    int lengthOfList = lengthList();
    if (lengthOfList > 0)
    {
        for(int i = 0; i < lengthOfList; i++){
            node *threadToDelete = deleteFirst();
            free(threadToDelete);
        }
    }
    _exit(3);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que realiza la conexion mediante un hilo, por lo cual debe ser un puntero
void *threaded(void *args)
{
    char buffer[30000];
    int socket = *((int*)args);
    free(args);
    
    read(socket, buffer, 30000);

    HTTPRequest request = HTTPRequest_constructor(buffer);
        
    if (request.Method == GET || request.Method == POST){

        if (request.Method == GET){
            char *copyRequestURI = malloc(sizeof(char) * (strlen(request.URI) + 1));
            strcpy(copyRequestURI, request.URI);

            char *parameter = malloc(sizeof(char) * (strlen(request.URI) + 1));
    
            strncpy(parameter, request.URI, 7);

            if(strcmp(parameter, PARAMETERKEY) == 0){
                char *fileName = malloc(sizeof(char) * (strlen(request.URI) + 1));
                strncpy(fileName, request.URI+7, strlen(request.URI));

                responseGet(socket,fileName,FOLDERPATH);   
                free(fileName);                     
            }
            free(copyRequestURI);
            free(parameter);

        }else if (request.Method == POST){
            responsePost();                 //Codigo para POST
        }
    }
    close(socket);
    pthread_mutex_lock(&listMutex);
    free(delete(pthread_self()));
    pthread_mutex_unlock(&listMutex);
    pthread_exit(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion para el manejo del pool de hilos
void *handle_pool(void *args){
    while(killFlag)
    {
        int *client;//en cada iteracion puede asignarse un cliente
        pthread_mutex_lock(&pool_mutex); //bloqueo el pool
        if((client = dequeue()) == NULL){ //si no hay request que desencolar se duerme el hilo
            pthread_cond_wait(&pool_cond,&pool_mutex);
            client = dequeue(); //se despierta y desencola cuando llega un request
        }
        pthread_mutex_unlock(&pool_mutex);//se desbloquea el pool
        if(client != NULL){ // se verifica que sea un socket de cliente valido
            int c = *((int*)client);
            FIFO(c); //se llama a FIFO
        }
        free(client);
    }
    pthread_exit(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que realiza el launch del servidor y elegir el tipo de servidor
void launch(Server *server)
{
    if(serverType.type == 3){ //si es un servidor de pool de hilos, se crean los hilos
        pool = malloc(serverType.threads*sizeof(pthread_t)); // hay que hacerle free
        for(int i = 0; i < serverType.threads; i++)
            pthread_create(&pool[i],NULL,handle_pool,NULL);
    }

    if(serverType.type == 5){ //si es un servidor de pool de procesos, se crean los procesos necesarios
        preforked(serverType.processes,server);
    }

    while(killFlag){
        printf("===== WAITING FOR CONNECTION =====\n");

        int address_length = sizeof(server->address);

        int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length);

        if(serverType.type == 1)
            FIFO(new_socket); //la conexion mas basica FIFO

        else if(serverType.type == 2){
            pthread_t t; //creacion de un hilo para cada request
            int *socket = malloc(sizeof(int)); //se asigna un socket
            *socket = new_socket;
            pthread_create(&t,NULL,threaded,socket); //se ejecuta el request del hilo al servidor
            pid_t test;
            insertFirst(t, t, test); //se guarda el id del hilo en la lista enlazada

        } else if(serverType.type == 3){ //pool de hilos
            int *socket = malloc(sizeof(int));
            *socket = new_socket;
            pthread_mutex_lock(&pool_mutex);
            enqueue(socket); //se encola el hilo
            pthread_cond_signal(&pool_cond); // se despierta al primero en la cola
            pthread_mutex_unlock(&pool_mutex);

        } else if(serverType.type == 4) { //un proceso
            pid_t childpid;
            if((childpid = fork()) == 0){
                forked(new_socket,childpid); 
            } else {
                pthread_t t;
                insertFirst(childpid, t, childpid);
                close(new_socket);
                wait(NULL);
            }
        }    
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que realiza hace el pool de procesos para el modo de preforked
void preforked(int nchildren, Server *server)
{
    int pid, newsock;
    for (int x = 0; x < nchildren; x++) { // Deja solo X cantidad de hilos vivir
        if ((pid = fork()) == 0){
            while (killFlag){ // Mientras no haya cambie el flag de kill
                printf("===== WAITING FOR CONNECTION =====\n");

                int address_length = sizeof(server->address);
                newsock = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length);
                FIFO(newsock);
                
                int lengthOfList = lengthList();
                if (lengthOfList > 0)
                {
                    for(int i = 0; i < lengthOfList; i++){
                        node *threadToDelete = deleteFirst();
                        free(threadToDelete);
                    }
                }
            }
        } else {
            pthread_t t;
            insertFirst(pid, t, pid);
        }
    }
    wait(NULL);
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
    if(serverType.type == 3){
        for(int i = 0; i < serverType.threads; i++){
            pthread_cancel(pool[i]);
        }
        free(pool);
    }
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
// Funcion que mata todos los procesos en caso de que haya que un mensaje de kill
void killProceces(){
    int lengthOfList = lengthList();
    if (lengthOfList > 0)
    {
        for(int i = 0; i < lengthOfList; i++){
            node *threadToDelete = deleteFirst();
            kill(threadToDelete->pid, SIGKILL);
            free(threadToDelete);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion main
int main(int argc, char**argv)
{
    int N;
    int T = 0;
    int P = 0;
    char consoleInput[30];

    N = atoi(argv[1]); // Lee el modo del servidor
    if (N == 3){ // Si es pre threaded o pre forked se ocupa un N
        T = atoi(argv[2]); 
    } else if (N == 5){
        P = atoi(argv[2]);
    }

    serverType.type = N;
    serverType.processes = P;
    serverType.threads = T;   

    killFlag = 1;

    pthread_create(&serverThread,NULL,serverFunc,NULL); // Crea el hilo donde esta funcionando todo

    while(killFlag){
        scanf("%s", consoleInput);
        if (strcmp(consoleInput, "kill") == 0){ // Si hay un mensaje por consola que se kill mata el servidor
            killFlag = 0;
        }
        strcpy(consoleInput,"");
    }

    close(server.socket);
    if (serverType.type == 2 || serverType.type == 3){
        killThreads();
    } else if (serverType.type == 4 || serverType.type == 5){
        killProceces();
    }

    return 0;
}