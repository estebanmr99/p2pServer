#include "HTTPRequest.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Funcion para determinar cual es el metodo HTTP del request
int chooseMethod(char *method){

    if (strcmp(method,"GET")==0)
        return GET;
    else if(strcmp(method,"POST")==0)
        return POST;
    else if(strcmp(method,"PUT")==0)
        return PUT;
    else if(strcmp(method,"DELETE")==0)
        return DELETE;
    else if(strcmp(method,"PATCH")==0)
        return PATCH;
    else if(strcmp(method,"HEAD")==0)
        return HEAD;
    else if(strcmp(method,"CONNECT")==0)
        return CONNECT;
    else if(strcmp(method,"OPTIONS")==0)
        return OPTIONS;
    else if(strcmp(method,"TRACE")==0)
        return TRACE;
    else
        return -1;
}

// Metodo para deconstruir el request en el struct de HTTPRequest
HTTPRequest HTTPRequest_constructor(char *request_str)
{
    char *request_line = strtok(request_str,"\n");

    HTTPRequest request;
    char *method = strtok(request_line," ");
    char *URI = strtok(NULL," ");
    char *HTTPVersion = strtok(NULL," ");
    HTTPVersion = strtok(HTTPVersion,"/");
    HTTPVersion = strtok(NULL,"/");
    request.Method = chooseMethod(method);
    request.URI = URI;
    request.HTTPVersion = (float)atof(HTTPVersion);

    return request;

}