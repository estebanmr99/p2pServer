#include "Request.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Funcion para determinar cual es el metodo HTTP del request
int chooseMethod(char *method){

    if (strcmp(method,"ONLINE") == 0)
        return ONLINE;
    else if(strcmp(method,"GET") == 0)
        return GET;
    else if(strcmp(method,"OFFLINE") == 0)
        return OFFLINE;
    else
        return -1;
}

char* copyString(char s[])
{
    char* s2;
    s2 = (char*)malloc(strlen(s));
 
    strcpy(s2, s);
    return (char*)s2;
}

// Parses out the request line to retrieve the method, uri, and http version.
void extract_request_line_fields(struct Request *request, char *request_line)
{
    // Copy the string literal into a local instance.
    char fields[strlen(request_line)];
    strcpy(fields, request_line);
    // Separate the string on spaces for each section.
    char *method = strtok(fields, " ");
    int methodEnum = chooseMethod(method);

    if (methodEnum == ONLINE || methodEnum == OFFLINE){
        char *ip = strtok(NULL," ");
        char *port = strtok(NULL," ");
        request->IP = copyString(ip);
        request->Port = copyString(port);
    } else if (methodEnum == GET){
        char *filename = strtok(NULL," ");
        request->FileName = copyString(filename);
    } 

    request->Method = methodEnum;
}

// Metodo para deconstruir el request en el struct de HTTPRequest
Request Request_constructor(char *request_str)
{
    char requested[strlen(request_str)];
    strcpy(requested, request_str);
    int found = 0;
    for (int i = 0; i < strlen(requested) - 2 && !found; i++)
    {
        if (requested[i] == '\n')
        {
            requested[i] = '|';
            found = 1;
        }
    }

    Request request;
    char *request_line = strtok(requested, "|");
    char *files = strtok(NULL, "|");

    // Parse each section as needed.
    extract_request_line_fields(&request, request_line);

    if(request.Method == ONLINE){
        request.Files = copyString(files);
    }

    return request;
}