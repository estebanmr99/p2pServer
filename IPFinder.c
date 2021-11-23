#include "IPFinder.h"

// Funcion para encontrar la IP local en los paquetes de IP
IPFinder finder_constructor(){
	IPFinder ipfinder;
	FILE *f;
    char line[100] , *p , *c;
    
	// Leer del archivo /proc/net/route para obtener las interfaces de red
    f = fopen("/proc/net/route" , "r");
    
    while(fgets(line, 100, f))
    {
		p = strtok(line , " \t");
		c = strtok(NULL , " \t");
		
		if(p!=NULL && c!=NULL)
		{
			if(strcmp(c , "00000000") == 0) // Si es "00000000" es porque se encontro la interfaz de red
			{
				break;
			}
		}
	}
    
    // se determina cual es la familia que se quiere, AF_INET or AF_INET6
    int fm = AF_INET;
    struct ifaddrs *ifaddr, *ifa;
	int family , s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) 
	{
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	// Itera por la lista vinculada, manteniendo el puntero principal para liberar la lista más tarde
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
	{
		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;
		if(strcmp( ifa->ifa_name , p) == 0)
		{
			if (family == fm) // Cuando encuentra la familia encuentr la IP
			{
				s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);
				
				if (s != 0) 
				{
					printf("getnameinfo() failed: %s\n", gai_strerror(s));
					exit(EXIT_FAILURE);
				}
				ipfinder.ip = host; 
				return ipfinder; // retorna la IP encontrada
			}
		}
	}
	freeifaddrs(ifaddr);
}