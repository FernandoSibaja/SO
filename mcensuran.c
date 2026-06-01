#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h> 
#include "mcensuran.h"


#define palabrasmaximas 100
#define longitudmaxima 100
#define listamax 5000
#define maxarchivo 50000

int comparar(char *texto, char *palabra);
void censurarpalabra(char *contenido, char *palabra);
int cargarlista(char *nombrelista, char palabras[palabrasmaximas][longitudmaxima]);
int mcensuran(char *arg[]);

int mcensuran(char *arg[])
{
    if(arg[1] == NULL || arg[2] == NULL)
    {
        return 1;
    }
    
    char *archivo = arg[1];
    char *lista = arg[2];

    char palabras[palabrasmaximas][longitudmaxima];

    int totalpalabras = cargarlista(lista, palabras);
    if(totalpalabras < 0)
    {
        printf("Error al cargar la lista de palabras\n");
        return 1;
    }
    int fd = open(archivo, O_RDONLY);
    if(fd < 0)
    {
        perror("Error al abrir el archivo");
        return 1;
    }
    char contenido[maxarchivo];
    int bytes = read(fd, contenido, sizeof(contenido)-1);
    if(bytes < 0)
    {
        perror("Error al leer el archivo");
        close(fd);
        return 1;
    }
    contenido[bytes] = '\0';
    close(fd);
    for(int i =0; i < totalpalabras; i++)
    {
        censurarpalabra(contenido, palabras[i]);
    }
    fd = open(archivo, O_WRONLY | O_TRUNC);
    if(fd < 0)
    {
        perror("Error al abrir el archivo para escribir");
        return 1;
    }
    write(fd,contenido, strlen(contenido));
    close(fd);
    printf("%s", contenido);
    return 0;
    

}

int comparar(char *texto, char *palabra)
{
    int i = 0;

    while(palabra[i] != '\0')
    {
        if(texto[i] == '\0')
            return 0;

        if(tolower(texto[i]) != tolower(palabra[i]))
            return 0;

        i++;
    }

    return 1;
}
void censurarpalabra(char *contenido, char *palabra)
{
    int longitud = strlen(palabra);
    char *posicion = contenido;
    while(*posicion != '\0')
    {
        if(comparar(posicion, palabra))
        {
            for(int i =0; i< longitud; i++)
            {
                posicion[i] = '*';
            }
            posicion += longitud;
        }
        else
        {
            posicion++;
        }
        
        
    }
}

int cargarlista(char *nombrelista, char palabras[palabrasmaximas][longitudmaxima])
{
    char buffer[listamax];
    int fd = open(nombrelista, O_RDONLY); 
    if(fd < 0)
    {
        perror("Error al abrir el archivo");
        return 0;
    }
    int bytes = read(fd,buffer, sizeof(buffer)-1);
    if(bytes < 0)
    {
        perror("Error al leer la lista");
        close(fd);
        return -1;
    }
    buffer[bytes] = '\0';
    close(fd);

    int total=0;
    int i=0;
    int j=0;
    while(buffer[i]!='\0' && total < palabrasmaximas) 
    {
        j=0; 
        while(buffer[i]!='\0' && buffer[i]!='\n' && j < longitudmaxima-1)
        {
            palabras[total][j] = buffer[i];
            i++;
            j++;
        }
        palabras[total][j] = '\0';
         if(buffer[i]=='\n')
         {
            i++;
         }
         if(strlen(palabras[total])>0)
         {
            total++;
         }
         
    }
    return total;
}

