#include "Peer.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fcntl.h>
#include <openssl/md5.h>
#include <string.h>

struct sockaddr_in server_address;
pthread_mutex_t mutex_aux = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;



pthread_t inputThread;

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion para concatenar dos punteros de char
char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = (char*)malloc(len1 + len2 + 1); 

    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); 
    return result;

}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion para retornar la cadena de chars inversa
void revstr(char *str1)
{

    int i, len, temp;
    len = strlen(str1); 

    for (i = 0; i < len/2; i++)
    {
        temp = str1[i];
        str1[i] = str1[len - i - 1];
        str1[len - i - 1] = temp;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion para castear el puerto a string
char* parse_port(long int port){

    char *result = "";

    while(port!=0){
        int auxn = port%10;

        if(auxn==0) result = concat(result,"0");

        else if(auxn==1) result = concat(result,"1");

        else if(auxn==2) result = concat(result,"2");

        else if(auxn==3) result = concat(result,"3");

        else if(auxn==4) result = concat(result,"4");

        else if(auxn==5) result = concat(result,"5");

        else if(auxn==6) result = concat(result,"6");

        else if(auxn==7) result = concat(result,"7");

        else if(auxn==8) result = concat(result,"8");

        else if(auxn==9) result = concat(result,"9");


        port /= 10;
    }

    revstr(result);

    return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que envia al servidor principal el request del ONLINE
char* get_files()
{

    char *buffer = "ONLINE ";

    buffer = concat(buffer,IP);

    buffer = concat(buffer," ");

    buffer = concat(buffer,parse_port(PORT));

    buffer = concat(buffer,"\n");

    unsigned char c[MD5_DIGEST_LENGTH];
    FILE *file;
    struct stat file_info;
    struct passwd *pw;
    char file_size[1024];
    MD5_CTX mdContext;
    int bytes;
    char hash[33];
    unsigned char data[1024];

    char *hash1 = "8222hnsnkai28bsmmsnsbssmssnwu2u";

    //NOMBREARCHIVO.png HASH TAMANO AUTOR\n
    DIR *d;
    struct dirent *dir;
    d = opendir("files");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir-> d_type != DT_DIR){ // if the type is not directory just print it with blue color

                char path[] = "files/";
                strcat(path,dir->d_name);
                file = fopen (path, "rb");
                fstat(fileno(file), &file_info);
                pw = getpwuid(file_info.st_uid);


                MD5_Init (&mdContext);
                while ((bytes = fread (data, 1, 1024, file)) != 0)
                    MD5_Update (&mdContext, data, bytes);
                MD5_Final (c,&mdContext);
                // for(int i = 0; i < MD5_DIGEST_LENGTH; i++) printf("%02x", c[i]);
                sprintf(hash,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], c[9], c[10], c[11], c[12], c[13], c[14], c[15] );

                buffer = concat(buffer,dir->d_name);
                buffer = concat(buffer," ");
                buffer = concat(buffer,hash);
                buffer = concat(buffer," ");
                sprintf(file_size, "%ld", file_info.st_size);
                buffer = concat(buffer,file_size);
                buffer = concat(buffer," ");
                buffer = concat(buffer,pw->pw_name);
                buffer = concat(buffer,"\n");

                fclose (file);
            }
    }
    closedir(d);
  }


  return buffer;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que lee una entrada hasta el salto de linea en consola
char* scan_line(char *line)
{
    int ch;             
    long capacity = 0;  
    long length = 0;    
    char *temp = NULL;  

    while ( ((ch = getchar()) != '\n') && (ch != EOF) )
    {
        if((length + 1) >= capacity)
        {
            
            if (capacity == 0)
                capacity = 2; 
            else
                capacity *= 2; 

            
            if( (temp = (char*)realloc(line, capacity * sizeof(char))) == NULL ) //allocating memory
            {
                printf("ERROR: unsuccessful allocation");
                
                exit(1);
            }

            line = temp;
        }

        line[length] = (char) ch; 
        length++;
    }
    line[length + 1] = '\0'; 

    
    if( (temp = (char*)realloc(line, (length + 1) * sizeof(char))) == NULL )
    {
        printf("ERROR: unsuccessful allocation");
        
        exit(1);
    }

    line = temp;
    return line;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que recibe el buffer del servidor principal de FIND
int file_list_request(){

    int peer_fd;


    //create socket
    if((peer_fd = socket(AF_INET, SOCK_STREAM , 0)) < 0){
        perror("Failed to create socket");
        exit(0);
    }

    //connect to server
    if(connect(peer_fd,(struct sockaddr*) &server_address, sizeof(server_address)) < 0){
        perror("Failed to connect");
        exit(0);
    }

    char *buffer = get_files();

    int numSent = send(peer_fd, buffer, 30000, 0);

        if (numSent <= 0){ //Si la conexion con el cliente se cerro deja de enviar los bytes
            if (numSent == 0){
                printf("The client was not written to: disconnected\n");
            } else {
                perror("The client was not written to");
            }
            return 0;
        }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que prepara el request de FIND
void prepare_request(char *response,char *file_name){
    
    FILE *fp = fopen(FILEPATH, "w"); // Abre archivo
    //printf("%s\n",file_name);
    fputs(file_name,fp);

    free(file_name);

    fputs("\n",fp);      
    char *st_result, *st_help;
    response = strtok_r(response, "\n", &st_result);
    while (response){
    char *help = strtok_r(response, " ", &st_help);
    help = strtok_r(NULL, " ", &st_help);
    int i = 0;    
    while (help) {
         fputs(help,fp);
         if(i!=4)fputs(" ",fp);
         help = strtok_r(NULL, " ", &st_help);
         i++;
    }
    //i++;
    fputs("\n",fp);
    response = strtok_r(NULL, "\n", &st_result);
    }
    fclose(fp);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion para calcular los segmentos de bytes para descargar
int calculate_chunk(long int size, int peers){

    int total = size / CHUNK_SIZE;
    
    if((CHUNK_SIZE * total) < size)
        total += 1;
    
    return total;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que se conecta con un peer y guarda en un buffer de bytes el segmento del archivo
void *send_request_to_peers(void *args){

    Peer peer = *((Peer*)args);

    
    //request 77789 2a888118171d99cc037ea060545076c4
    int sock_fd;

    struct sockaddr_in peer_address;

    peer_address.sin_family = PF_INET;
    peer_address.sin_addr.s_addr = inet_addr(peer.address);
    peer_address.sin_port = htons(peer.port);

    //create socket
    if((sock_fd = socket(AF_INET, SOCK_STREAM , 0)) < 0){
        perror("Failed to create socket");
        exit(0);
    }

    //connect to server
    if(connect(sock_fd,(struct sockaddr*) &peer_address, sizeof(peer_address)) < 0){
        perror("Failed to connect");
        exit(0);
    }
    
    //nombre archivo
    //printf("FILE: %s\n",peer.file_name);
    send(sock_fd,&peer.file_name ,sizeof(peer.file_name), 0);
    //hash
    send(sock_fd,&peer.hash,sizeof(peer.hash),0);
    //size
    send(sock_fd,&peer.size,sizeof(peer.size),0);
    //index
    send(sock_fd,&peer.index,sizeof(peer.index),0);
    //chunk
    send(sock_fd,&peer.chunk,sizeof(peer.chunk),0);

    //recibe el buffer de bytes
    char buffer[CHUNK_SIZE];

    //P(324);
    printf("--->INICIA EL PEER: %ld\n",peer.chunk);
    recv(sock_fd, &buffer, sizeof(buffer), 0);
    //printf("recibe el buffer el cliente\n");
    //printf("Index: %d\n",peer.index);

    //P(333);
    //pthread_mutex_lock(&mutex_aux);
    printf("CLIENTE: INDEX: %ld PEER: %d\n",peer.chunk,peer.index);
    FILE *fp = fopen(parse_port((long)peer.chunk), "wb");
    printf("CLIENTE %ld : GUARDA EL ARCHIVO\n",peer.chunk);
    //P(335);
    fwrite(buffer , sizeof(buffer) ,1 , fp);
    printf("CLIENTE %ld : ESCRIBE EL BUFFER EN EL ARCHIVO\n",peer.chunk);
    //P(337);
    fclose(fp);
    printf("CLIENTE %ld : CIERRA EL ARCHIVO\n",peer.chunk);
    //pthread_mutex_unlock(&mutex_aux);

    
    //sleep(5);
    
    close(sock_fd);
    printf("CLIENTE %ld : CIERRA EL SOCKET\n",peer.chunk);

    pthread_exit(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que arma el request GET al servidor principal
int get_file(char *file_name){

    char *buffer = "GET ";

    buffer = concat(buffer,file_name);

    //printf("%s\n",buffer);

    int peer_fd;

    //create socket
    if((peer_fd = socket(AF_INET, SOCK_STREAM , 0)) < 0){
        perror("Failed to create socket");
        exit(0);
    }

    //connect to server
    if(connect(peer_fd,(struct sockaddr*) &server_address, sizeof(server_address)) < 0){
        perror("Failed to connect");
        exit(0);
    }

    char response[30000];
    int numSent = send(peer_fd, buffer, 30000, 0);
    read(peer_fd, response, 30000);

    printf("%s", response);

    prepare_request(response, file_name);

    if (numSent <= 0){ //Si la conexion con el cliente se cerro deja de enviar los bytes
        if (numSent == 0){
            printf("The client was not written to: disconnected\n");
        } else {
            perror("The client was not written to");
        }
        return 0;
    }

}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion para copiar un string
char* copyString(char s[])
{
    char* s2;
    s2 = (char*)malloc(strlen(s));
 
    strcpy(s2, s);
    return (char*)s2;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que encuentra los peers que tienen el archivo 
void find_peers(char* size,char* hash){

    int peers_count = 0;
    char *file_name;
    char *peers = (char *)calloc(1, sizeof(char));


    if(access(FILEPATH, 0 ) == 0){ // Si el achivo existe en la ruta de archivos
        FILE *fp = fopen(FILEPATH, "r");
        int lineLength = 360;
        char line[lineLength];

        int flag = 1;

        while(fgets(line, sizeof(line), fp)) {
            char *copyLine = copyString(line);
            if(flag){
                file_name = copyString(copyLine);

                //peers = concat(peers,file_name);
  
                flag = 0;
               
            }

            char *token = strtok(line," ");//hash

            char *aux_hash = copyString(token);

            if(strcmp(hash,aux_hash)==0){
                token = strtok(NULL," ");//tamanio
                if(strcmp(size,token)==0){

                    peers_count++;

                    token = strtok(NULL," ");//se ignora el autor
                    token = strtok(NULL," ");//IP
                    peers = concat(peers,token);
                    peers = concat(peers," ");//separador espacio
                    token = strtok(NULL," ");//PORT
                    peers = concat(peers,token);//incluye separador
                }
                
            }

            free(aux_hash);
            

            free(copyLine);
        }

        fclose(fp);
        
        //printf("Buffer: \n%s\n Total de peers: %d\n",peers,peers_count);


    }

    Peer *peers_list = malloc(sizeof(Peer)*peers_count);

    long int chunk_size = calculate_chunk((long)atoi(size),peers_count);
    
    char *st_result, *st_help;

    peers = strtok_r(peers, "\n", &st_result);

    int i = 0;
    while (peers){

        char *help = strtok_r(peers, " ", &st_help);

        peers_list[i].address = help;//IP

        peers_list[i].file_name = copyString(file_name);

        peers_list[i].size = (long)atoi(size);

        peers_list[i].index = i;

        peers_list[i].chunk = chunk_size;

        peers_list[i].hash = copyString(hash);

        help = strtok_r(NULL, " ", &st_help); //PORT
      
        help[strcspn(help, "\n")] = 0; //quitar salto de linea para usar atoi
        
        peers_list[i].port = atoi(help);

        peers = strtok_r(NULL, "\n", &st_result);
        i++;

    }

    free(file_name);
    free(size);
    free(hash);

    //asignar fuera el indice que le corresponde

    int total_chunks = calculate_chunk(peers_list[0].size,peers_count);

    Peer *peers_requests = malloc(sizeof(Peer)*total_chunks);

    for(int i=0;i<total_chunks;i++){
        int index = rand() % peers_count;//determinar el peer
        peers_requests[i].address = peers_list[index].address;
        peers_requests[i].port = peers_list[index].port;
        peers_requests[i].file_name = peers_list[index].file_name;
        peers_requests[i].hash = peers_list[index].hash;
        peers_requests[i].size = peers_list[index].size;
        peers_requests[i].index = index;
        peers_requests[i].chunk = i;
    }

    pthread_t peers_threads[total_chunks];
    for(int i=0; i<total_chunks;i++){
        printf("PEER: index: %ld peer: %d\n",peers_requests[i].chunk,peers_requests[i].index);
        //pthread_mutex_lock(&mutex_aux);

        pthread_create(&peers_threads[i],NULL,send_request_to_peers, &peers_requests[i]);//se llama el peer que se eligio
        //pthread_join(peers_threads[i],NULL);//se llama el peer que se eligio
    }
    //for(int i=0; i<total_chunks;i++){

    //    pthread_join(peers_threads[i],NULL);//se llama el peer que se eligio
    //}

}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que parsea el input del usuario
void parse_command(char *user_line){

    char *ptr = strtok(user_line," ");

    char *command = copyString(ptr);

    if(strcmp(command,"find")==0){


        ptr = strtok(NULL," ");

        char *file_name;

        file_name = copyString(ptr);

        //strcpy(file_name,ptr);


        get_file(file_name);

        free(user_line);


    }

    else if(strcmp(command,"request")==0){
        char *size,*hash;
        ptr = strtok(NULL," ");

        //size = (char *)malloc(strlen(ptr)*sizeof(char));

        size = copyString(ptr);

        ptr = strtok(NULL," ");

        //hash = (char *)malloc(strlen(ptr)*sizeof(char));

        hash = copyString(ptr);

        printf("command: %s\nsize: %s\nhash: %s\n",command,size,hash);

        find_peers(size,hash);

        free(user_line);

    }
    else if(strcmp(command,"clear")==0)
    {
        system("clear");

    }

    else{
        printf("command not found!\n");
        free(user_line);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion del ciclo de la consola
void *client_input(void *args){

    while(1){
        printf(">");

        char *a = NULL;

        a = scan_line(a); //function call to scan the line
        parse_command(a);
    }
}



//---------------------------------------------------------------------------------------------------------------------------------------
// Funcion que arma el buffer del archivo a enviar como peer
void *handle_request(void *args){

    int peer_fd = *((int*)args);
    free(args);
    
    char *file_name,*hash;
    long int size,chunk;
    int index;

    recv(peer_fd, &file_name, sizeof(file_name), 0);
    recv(peer_fd, &hash, sizeof(hash), 0);
    recv(peer_fd, &size, sizeof(size), 0);
    recv(peer_fd, &index, sizeof(index), 0);
    recv(peer_fd, &chunk, sizeof(chunk), 0);

    printf("CHUNK: %ld : PEER: %d\n",chunk,index);

    char *path = "files/";
    concat(path,file_name);

    FILE *fp = fopen(path,"rb");
    printf("PEER : %ld ABRE EL ARCHIVO\n",chunk);

    char buffer[CHUNK_SIZE];
    int i = CHUNK_SIZE * chunk;

    fseek(fp,i,SEEK_SET);//el stream apunte al inicio de los bytes del chunk
    printf("PEER : %ld INDEXA EL ARCHIVO\n",chunk);

    //printf("lee desde este punto\n");
    fread(buffer,sizeof(buffer),1,fp);
    printf("PEER : %ld LLENA EL BUFFER\n",chunk);

    fclose(fp);
    printf("PEER : %ld CIERRA EL ARCHIVO\n",chunk);


    send(peer_fd,&buffer,sizeof(buffer), 0);
    printf("PEER : %ld ENVIA EL BUFFER\n",chunk);

    close(peer_fd);

    pthread_exit(0);
}


void launch(Client *client)
{

    struct sockaddr_in client_address, peer_address;

    int server_fd;

    //Inicializar servidor principal
    server_address.sin_family = PF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);


    //Inicializar peer
    peer_address.sin_family = PF_INET;
    peer_address.sin_addr.s_addr = INADDR_ANY;
    peer_address.sin_port = htons(PORT);


    file_list_request();

    pthread_create(&inputThread,NULL,client_input,NULL); // Crea el hilo donde esta funcionando todo

    //El cliente escucha

    while(1){

        int client_address_length = sizeof(client->address); //es del struct sockaddr_in

        int process_fd = accept(client->socket, (struct sockaddr *) &client->address, (socklen_t *) &client_address_length);

        pthread_t t; //creacion de un hilo para cada request
        int *socket = (int*)malloc(sizeof(int)); //se asigna un socket
        *socket = process_fd;
        pthread_create(&t,NULL,handle_request,socket); //se ejecuta el request del hilo al servidor
        //pthread_join(t,NULL);
    }

}


int main(){

    Client client = client_constructor(AF_INET,SOCK_STREAM, 0, IP, PORT, 5000, launch);

    /*
    FILE *fp;
    struct stat file_info;
    fp = fopen("files/meme.jpeg", "rb"); // Abre archivo

    fstat(fileno(fp), &file_info);

    fseek(fp,0,SEEK_SET);
    char buffer[file_info.st_size];

    //fread(&buffer,file_info.st_size, 1, fp);

    //printf("%s\n",buffer);

    //fclose(fp);

    //FILE * f = fopen("files/test.jpeg", "wb" );

    //fwrite(buffer , file_info.st_size ,1 , f);

    fclose(f);
    */

    client.launch(&client);

    return 0;
}
