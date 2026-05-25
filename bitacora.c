#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

#include "bitacora.h"

int bitacora(char *arg[])
{
    char *archivo = arg[1];
    char *mensaje = arg[2];

    int fd = open(archivo, O_WRONLY | O_CREAT | O_APPEND, 0644);
     if(fd < 0)
     {
        perror("Error al abrir el archivo");
        return 1;
        
     }
    char *directorio[1024];
    if(getcw(directorio, sizeof(directorio)) == NULL)
    {
        perror("Error al obtener el directorio actual");
        close(fd);
        return 1;
    }
    uid_t uid = getuid();
    char *usuario = getenv("USER");
    time_t tiempo =time(NULL);

    
}