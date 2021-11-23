#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// Tamaño por defecto del buffer
#define BUF_SIZE 1024

// Funciones del programa
int create_request(char * dir, char * port, char * R);
int isValidIP(char * ip);
void * client();
void mostrar_estadisticas();

// Variables globales del programa
// Parametros ingresados por consola
char * dir, * R, * P;
int T, H;
// Buffer para la lectura del archivo
char buffer[BUF_SIZE];
// Variables compartidas entre hilos
time_t *tiempos_espera;
time_t *tiempos_atencion;
int indx_espera;
int indx_atencion;
// Conteo de bytes
ssize_t received;
// Mutex para control de variables compartidas
pthread_mutex_t received_mutex;
pthread_mutex_t indx_espera_mutex;
pthread_mutex_t indx_atencion_mutex;

// Funcion principal del programa
// Inicializa las variables y crea los hilos
int main(int argc, char**argv) {

    // Se inicializan la variables globales del programa
    T = atoi(argv[1]);  // Cantidad de solicitudes por hilo
    H = atoi(argv[2]);  // Cantidad de hilos
    R = argv[3];    // Recurso a solicitar al servidor
    dir = argv[4];  // Direccion del servidor
    P = argv[5];    // Puerto al que escucha el servidor
    time_t te[T*H];
    tiempos_espera = te;    // Lista con los tiempos de espera de cada hilo
    time_t ta[T*H];
    tiempos_atencion = ta;  // Lista con los tiempos de atencion de cada hilo
    received = 0;   // Cantidad de bytes recibidos
    indx_espera = 0;    // Indice para la lista con tiempos de espera
    indx_atencion = 0;  // Indice para la lista con tiempos de atencion

    pthread_t threads[H];   // Lista de hilos

    // Se inicializan mutex para el control de variables compartidas
    pthread_mutex_init(&received_mutex, NULL);
    pthread_mutex_init(&indx_espera_mutex, NULL);
    pthread_mutex_init(&indx_atencion_mutex, NULL);

    // Se crean los hilos
    for (int i = 0; i < H; i++)
        pthread_create(&threads[i],NULL,client,NULL);
    for (int i = 0; i < H; i++)
        pthread_join(threads[i],NULL);

    // Se destruyen los mutex una vez se terminan los hilos
    pthread_mutex_destroy(&received_mutex);
    pthread_mutex_destroy(&indx_espera_mutex);
    pthread_mutex_destroy(&indx_atencion_mutex);

    // Se muestran las estadisticas
    mostrar_estadisticas();

    return 0;
}

// Funcion que envia la solicitud GET y recibe el archivo solicitado
void * client(){

    int sockfd;
    long int temp;
    time_t inicio_atencion;
    time_t fin_atencion;

    // Realiza T solicitudes
    for (int i = 0; i < T; i++) {
        sockfd = create_request(dir, P, R);

        // Empieza tiempo atencion
        inicio_atencion = time(NULL);
        // sleep(3); // Para probar con pocas solicitudes

        // Se recibe el archivo
        memset(&buffer, 0, sizeof(buffer));
        temp = recv(sockfd, buffer, BUF_SIZE, 0);
        while (temp > 0) {
            pthread_mutex_lock(&received_mutex);
            received+=temp;
            pthread_mutex_unlock(&received_mutex);
            temp = recv(sockfd, buffer, BUF_SIZE, 0);
            memset(&buffer, 0, sizeof(buffer));
        }

        // Termina tiempo de atencion
        fin_atencion = time(NULL);

        // Se calcula el tiempo de atencion y se guarda
        pthread_mutex_lock(&indx_atencion_mutex);
        tiempos_atencion[indx_atencion] = fin_atencion  - inicio_atencion;
        indx_atencion++;
        pthread_mutex_unlock(&indx_atencion_mutex);

        close(sockfd);
    }
}

// Funcion para establecer la conexion con el servidor y crear
// la solicitud de tipo GET
int create_request(char * dir, char * port, char * R) {

    int sockfd;
    char getrequest[2048];
    struct sockaddr_in addr;
    time_t inicio_espera;
    time_t fin_espera;

    // Valida la direccion IP
    if (isValidIP(dir)) {
        sprintf(getrequest, "GET /?file=%s HTTP/1.0\nHOST: %s\n\n", R, dir);
    }

    // Crea un socket al host
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error creating socket!\n");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(dir);
    addr.sin_port = htons(atoi(port));

    //inicia tiempo espera
    inicio_espera = time(NULL);
    // sleep(2); // Para probar con pocas solicitudes

    // Solicita conexion con el servidor
    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0 ) {
        printf("Connection Error!\n");
        exit(1);
    }

    // Termina tiempo espera
    fin_espera = time(NULL);

    // Se calcula el tiempo de espera y se guarda
    pthread_mutex_lock(&indx_espera_mutex);
    tiempos_espera[indx_espera] = fin_espera - inicio_espera;
    indx_espera++;
    pthread_mutex_unlock(&indx_espera_mutex);

    // Escribe la solicitud al sockfd
    write(sockfd, getrequest, strlen(getrequest));

    return sockfd;
}

// Funcion para el calculo e impresion de las estadisticas
void mostrar_estadisticas() {

    int total_requests = T*H;
    long double total_espera=0;
    long double total_atencion=0;
    long double espera_promedio=0;
    long double atencion_promedio=0;
    long double varianza_espera=0;
    long double varianza_atencion=0;

    for (int i = 0; i < total_requests; i++){
        total_atencion += tiempos_atencion[i];  // Calculo del total de tiempo de atencion
        total_espera += tiempos_espera[i];  // Calculo del total de tiempo de espera
        // Calculo de la varianza en tiempos de atencion
        varianza_atencion += (tiempos_atencion[i] - atencion_promedio) * (tiempos_atencion[i] - atencion_promedio);
        // Calculo de la varianza en tiempos de espera
        varianza_espera += (tiempos_espera[i] - espera_promedio) * (tiempos_espera[i] - espera_promedio);
    }
    atencion_promedio = total_atencion / total_requests;    // Calculo de tiempo de atencion promedio
    espera_promedio = total_espera / total_requests;    // Calculo de tiempo de espera promedio
    varianza_atencion /= total_requests;    // Calculo de la varianza en tiempos de atencion
    varianza_espera /= total_requests;  // Calculo de la varianza en tiempos de espera

    // Se muestran las estadisticas en pantalla
    printf("\nTiempo total de atencion: %Lf segundos\n", total_atencion);
    printf("Tiempo total de espera: %Lf segundos\n", total_espera);
    printf("Tiempo de atencion promedio: %Lf segundos\n", atencion_promedio);
    printf("Tiempo de espera promedio: %Lf segundos\n", espera_promedio);
    printf("Varianza en el tiempo de atencion: %Lf\n", varianza_atencion);
    printf("Varianza de tiempos de espera: %Lf\n", varianza_espera);
    printf("Bytes recibidos: %ld\n\n", received);
}

// Funcion que valida que la IP brindada sea valida
int isValidIP(char * ip) {
    struct sockaddr_in addr;
    int valid = inet_pton(AF_INET, ip, &(addr.sin_addr));
    return valid != 0;
}
