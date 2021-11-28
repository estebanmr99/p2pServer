#ifndef HTTPRequest_h

#define HTTPRequest_h

// ENUM para clasificar los diferentes tipos de metodos HTTP
typedef enum HTTPMethods
{
    GET,
    POST,
    PUT,
    HEAD,
    DELETE,
    PATCH,
    CONNECT,
    OPTIONS,
    TRACE
}HTTPMethods;

// Struct para guardar la informacion de requests
typedef struct HTTPRequest
{
    int Method;
    char *URI;
    float HTTPVersion;
}HTTPRequest;

// Metodo para deconstruir el request en el struct de HTTPRequest
HTTPRequest HTTPRequest_constructor(char *request_str);

#endif