#include "Peer.h"
#include <dirent.h> 
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct sockaddr_in server_address;


pthread_t inputThread;

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

    char *result = malloc(sizeof(char)*4);


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


    char *hash = "8222hnsnkai28bsmmsnsbssmssnwu2u";
    //NOMBREARCHIVO.png HASH TAMANO AUTOR\n
    DIR *d;
    struct dirent *dir;
    d = opendir("files");
    printf("----------------------------\n");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir-> d_type != DT_DIR){ // if the type is not directory just print it with blue color
                buffer = concat(buffer,dir->d_name);
                buffer = concat(buffer," ");
                buffer = concat(buffer,hash);
                buffer = concat(buffer,"\n");   
            }
    }
    closedir(d);
  }

  printf("%s\n",buffer);



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
            if( (temp = realloc(line, capacity * sizeof(char))) == NULL ) //allocating memory
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
    if( (temp = realloc(line, (length + 1) * sizeof(char))) == NULL )
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

int get_file(char *file_name){

    char *buffer = "GET ";

    buffer = concat(buffer,file_name);

    printf("%s\n",buffer);

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

    char test[30000];
    int numSent = send(peer_fd, buffer, 30000, 0);
    read(peer_fd, test, 30000);
    printf("%s", test);

        if (numSent <= 0){ //Si la conexion con el cliente se cerro deja de enviar los bytes
            if (numSent == 0){
                printf("The client was not written to: disconnected\n");
            } else {
                perror("The client was not written to");
            }
            return 0;
        }

}

void parse_command(char *user_line){

    char *command;

    command = (char *)malloc(strlen(user_line)*sizeof(char));

    char *ptr = strtok(user_line," ");

    strcpy(command,ptr);

    if(strcmp(command,"find")==0){

        
        ptr = strtok(NULL," ");

        char *file_name;

        file_name = (char *)malloc(strlen(ptr)*sizeof(char));


        //char *file_name = malloc(sizeof(char)*strlen(ptr));

        strcpy(file_name,ptr);
        printf("command: %s\nfile name: %s\n",command,file_name);


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

        free(user_line);

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
    file_list_request();

    pthread_create(&inputThread,NULL,client_input,NULL); // Crea el hilo donde esta funcionando todo

    //El cliente escucha

    while(1){
        
        int client_address_length = sizeof(client->address); //es del struct sockaddr_in

        int process_fd = accept(client->socket, (struct sockaddr *) &client->address, (socklen_t *) &client_address_length);

        pthread_t t; //creacion de un hilo para cada request
        int *socket = malloc(sizeof(int)); //se asigna un socket
        *socket = process_fd;
        pthread_create(&t,NULL,handle_request,socket); //se ejecuta el request del hilo al servidor
    }

}

int main(){

    Client client = client_constructor(AF_INET,SOCK_STREAM, 0, IP, PORT, 5000, launch);

    client.launch(&client);

    return 0;
}