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


  return buffer;
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

    printf("%s",buffer);
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

void parse_command(int command){
    printf("Entra a parse command");

    
}



void *client_input(void *args){

    while(1){
        int command;
        printf(">");
        scanf("%d",&command);
        printf("command: %d",command);
        parse_command(command);
    }
}

void launch(Client *client)
{
    server_address.sin_family = PF_INET; 
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP); 
    server_address.sin_port = htons(SERVER_PORT); 

    int process_fd;
    int server_fd;
    struct sockaddr_in client_address, peer_address;

    //pthread_create(&inputThread,NULL,client_input,NULL); // Crea el hilo donde esta funcionando todo
    //pthread_join(inputThread,NULL);


    
    get_files();

    file_list_request();



    //while(1){
    //    
    //    int client_address_length = sizeof(client_address); //es del struct sockaddr_in
    //
    //
    //    if((process_fd = accept(server_fd,(struct sockaddr*) &client_address, (socklen_t *) &client_address_length)) < 0){
    //        perror("Failed to accept"); 
    //        exit(0);
    //    }
        
        //else{
        //    std::string file_name;
        //    int chunk_index;
        //    uint32_t request_type;
        //    recv(process_fd, &file_name, sizeof(file_name), 0);
        //    recv(process_fd, &chunk_index, sizeof(chunk_index), 0);
        //    std::cout<<"upload file:" << file_name <<"," <<chunk_index<<std::endl;
        //    //use multi-thread to handle requests
        //    std::thread tid(&client::handle_request, myclient, process_fd, file_name, chunk_index);
        //    tid.detach();
        //}
    //}

}

int main(){

    Client client = client_constructor(AF_INET,SOCK_STREAM, 0, IP, PORT, 5000, launch);

    client.launch(&client);

    return 0;
}