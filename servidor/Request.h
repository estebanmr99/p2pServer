#ifndef Request_h

#define Request_h

// ENUM para clasificar los diferentes tipos de metodos HTTP
typedef enum Methods {
    ONLINE,
    GET,
    OFFLINE
} Methods;

// Struct para guardar la informacion de requests
typedef struct Request {
    int Method;
    char *FileName;
    char *Files;
    char *IP;
    char *Port;
} Request;

// Metodo para deconstruir el request en el struct de HTTPRequest
Request Request_constructor(char *request_str);

char* copyString(char s[]);

#endif