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
#include "LinkedList.h"

struct sockaddr_in server_address;


File *file_list; 


pthread_t inputThread;


void restart_list(){
    file_list = malloc(sizeof(File)); //
    file_list->file_name = malloc(sizeof(char *));

    //file_list->isEmpty = malloc(sizeof(int));
    file_list->files = malloc(sizeof(LinkedList));
    file_list->isEmpty = 1; //vacio no se puede hacer request a los peers

}

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = (char*)malloc(len1 + len2 + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
    return result;

}

void revstr(char *str1)
{
    // declare variable
    int i, len, temp;
    len = strlen(str1); // use strlen() to get the length of str string

    // use for loop to iterate the string
    for (i = 0; i < len/2; i++)
    {
        // temp variable use to temporary hold the string
        temp = str1[i];
        str1[i] = str1[len - i - 1];
        str1[len - i - 1] = temp;
    }
}


char* parse_port(){

    char *result = (char*)malloc(sizeof(char)*4);


    int port = PORT;

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


char* get_files()
{

    char *buffer = "ONLINE ";

    buffer = concat(buffer,IP);

    buffer = concat(buffer," ");

    buffer = concat(buffer,parse_port());

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


int online(){

    int peer_fd;
    uint16_t number_of_file;

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

    char *buffer = "ONLINE ";

    buffer = concat(buffer,IP);

    buffer = concat(buffer," ");

    buffer = concat(buffer,parse_port());

    buffer = concat(buffer,"\n");

    printf("%s\n",buffer);

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


char* scan_line(char *line)
{
    int ch;             // as getchar() returns `int`
    long capacity = 0;  // capacity of the buffer
    long length = 0;    // maintains the length of the string
    char *temp = NULL;  // use additional pointer to perform allocations in order to avoid memory leaks

    while ( ((ch = getchar()) != '\n') && (ch != EOF) )
    {
        if((length + 1) >= capacity)
        {
            // resetting capacity
            if (capacity == 0)
                capacity = 2; // some initial fixed length
            else
                capacity *= 2; // double the size

            // try reallocating the memory
            if( (temp = (char*)realloc(line, capacity * sizeof(char))) == NULL ) //allocating memory
            {
                printf("ERROR: unsuccessful allocation");
                // return line; or you can exit
                exit(1);
            }

            line = temp;
        }

        line[length] = (char) ch; //type casting `int` to `char`
        length++;
    }
    line[length + 1] = '\0'; //inserting null character at the end

    // remove additionally allocated memory
    if( (temp = (char*)realloc(line, (length + 1) * sizeof(char))) == NULL )
    {
        printf("ERROR: unsuccessful allocation");
        // return line; or you can exit
        exit(1);
    }

    line = temp;
    return line;
}



int file_list_request(){

    int peer_fd;
    int request_type = 1;
    uint16_t number_of_file;
    //std::string file_name;
    uint32_t file_length;

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

    //printf("%s",buffer);
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

void find_file(char *name_file){

    printf("%s\n",name_file);

}

void request_file(char *file_info){
    printf("%s\n",file_info);
}

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

char* copyString(char s[])
{
    char* s2;
    s2 = (char*)malloc(strlen(s));
 
    strcpy(s2, s);
    return (char*)s2;
}


void find_peers(char* size,char* hash){

    int peers_count = 0;
    char *file_name;

    if(access(FILEPATH, 0 ) == 0){ // Si el achivo existe en la ruta de archivos
        FILE *fp = fopen(FILEPATH, "r");
        int lineLength = 360;
        char line[lineLength];

        

        char *peers = (char *)calloc(1, sizeof(char));

        int flag = 1;

        while(fgets(line, sizeof(line), fp)) {
            char *copyLine = copyString(line);
            if(flag){
                file_name = copyString(copyLine);
                peers = concat(peers,file_name);
                //peers = concat(peers,"\n");//guardo el nombre del archivo
                peers = concat(peers,hash); //se guarda el hash
                peers = concat(peers,"\n"); //separador \n
                peers = concat(peers,size); //se guarda el tamanio
                peers = concat(peers,"\n"); //separador \n        
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
        free(file_name);
        free(size);
        free(hash);

        
        printf("Buffer: \n%s\n Total de peers: %d\n",peers,peers_count);
    }

}

void parse_command(char *user_line){

    //printf("%s\n",user_line);

    char *command;

    command = (char *)malloc(strlen(user_line)*sizeof(char));

    char *ptr = strtok(user_line," ");

    strcpy(command,ptr);

    if(strcmp(command,"find")==0){


        ptr = strtok(NULL," ");

        char *file_name;

        file_name = (char *)malloc(strlen(ptr)*sizeof(char));

        strcpy(file_name,ptr);


        get_file(file_name);

        free(user_line);


    }

    else if(strcmp(command,"request")==0){
        char *size,*hash;
        ptr = strtok(NULL," ");

        size = (char *)malloc(strlen(ptr)*sizeof(char));

        strcpy(size, ptr);
        ptr = strtok(NULL," ");
        hash = (char *)malloc(strlen(ptr)*sizeof(char));

        strcpy(hash, ptr);

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



void *client_input(void *args){

    while(1){
        printf(">");

        char *a = NULL;

        a = scan_line(a); //function call to scan the line


        parse_command(a);
    }
}




void *handle_request(void *args){
    int socket = *((int*)args);
    free(args);
    printf("CONECTADO\n");

    char buffer[30000];

    read(socket, buffer, 30000);

    printf("%s",buffer);

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


    //online();


    //get_files();

    //terminal de cliente para comandos
    //file_list_request();

    pthread_create(&inputThread,NULL,client_input,NULL); // Crea el hilo donde esta funcionando todo

    //El cliente escucha

    while(1){

        int client_address_length = sizeof(client->address); //es del struct sockaddr_in

        int process_fd = accept(client->socket, (struct sockaddr *) &client->address, (socklen_t *) &client_address_length);

        pthread_t t; //creacion de un hilo para cada request
        int *socket = (int*)malloc(sizeof(int)); //se asigna un socket
        *socket = process_fd;
        pthread_create(&t,NULL,handle_request,socket); //se ejecuta el request del hilo al servidor
    }

}


int main(){

    Client client = client_constructor(AF_INET,SOCK_STREAM, 0, IP, PORT, 5000, launch);

    restart_list();


    client.launch(&client);

    return 0;
}
