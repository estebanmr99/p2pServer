#include "Peer.h"

struct sockaddr_in server_address;


pthread_t inputThread;


void file_list_request(){

    int peer_fd;
    const uint32_t request_type = 1;
    uint16_t number_of_file;
    //std::string file_name;
    uint32_t file_length;

    printf("HOLA ACA");
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

    printf("SEND");
    send(peer_fd, &request_type, sizeof(request_type), 0); 

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
    int process_fd;
    int server_fd;
    struct sockaddr_in client_address, peer_address;

    //pthread_create(&inputThread,NULL,client_input,NULL); // Crea el hilo donde esta funcionando todo
    //pthread_join(inputThread,NULL);
    printf("hOLA MAIN");
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

    printf("HOLA");
    Client client = client_constructor(AF_INET,SOCK_STREAM, 0, IP, PORT, 5000, launch);
    printf("cliente construido");
    client.launch(&client);

    return 0;
}