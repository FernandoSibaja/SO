#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 

#include "encriptar.h"

#define bloques 16
#define rondas 4

void inicializarTablas();

void clavexor(unsigned char bloque[], char *clave, int ronda);

void sustituir(unsigned char bloque[]);

void sustituirinv(unsigned char bloque[]);

void permutar(unsigned char bloque[]);

void permutarinv(unsigned char bloque[]);

void cifrarbloque(unsigned char bloque[], char *clave);

void descifrarbloque(unsigned char bloque[], char *clave);

int cifrararchivo(char *entrada, char *salida, char *clave);

int descifrararchivo(char *entrada, char *salida, char *clave);

int encriptar(char *arg[]);

unsigned char sustitucion[256];
unsigned char sustitucion_inversa[256];

int permutacion[bloques] = {
    3, 0, 7, 4,
    1, 6, 2, 5,
    11, 8, 15, 12,
    9, 14, 10, 13
};

int invpermutacion[bloques];

int encriptar(char *arg[])
{
    if(arg[1] == NULL || arg[2] == NULL || arg[3] == NULL || arg[4] == NULL)
    {
        return 1;
    }

    inicializarTablas();
    if(strlen(arg[4])==0)
    {
        fprintf(stderr, "Error: La clave no puede estar vacía.\n");
        return 1;
    }
    if(strcmp(arg[1],"-e")==0)
    {
        return cifrararchivo(arg[2],arg[3],arg[4]);
    }
    else if(strcmp(arg[1],"-d")==0)
    {
        return descifrararchivo(arg[2],arg[3],arg[4]);
    }
    else
    {
        fprintf(stderr, "Error: Opción no válida. Use -e para cifrar o -d para descifrar.\n");
        return 1;
    }
}

void inicializarTablas()
{
    for(int i=0; i<256; i++)
    {
        sustitucion[i]=(unsigned char)((i*17+43) % 256);
    }
    for(int i=0; i<256; i++)
    {
        sustitucion_inversa[sustitucion[i]]=i;
    }
    for(int i=0; i<bloques; i++)
    {
        invpermutacion[permutacion[i]]=i;
    }
}

void clavexor(unsigned char bloque[], char *clave, int ronda)
{
    int logintudclave = strlen(clave);
    for(int i=0; i<bloques; i++)
    {
        bloque[i]^=clave[(i+ronda) % logintudclave];
    }
}

void sustituir(unsigned char bloque[])
{
    for(int i=0; i<bloques;i++)
    {
        bloque[i]=sustitucion[bloque[i]];
    }
}
void sustituirinv(unsigned char bloque[])
{
    for(int i=0; i<bloques;i++)
    {
        bloque[i]=sustitucion_inversa[bloque[i]];
    }
}
 void permutar(unsigned char bloque[])
 {
    unsigned char temp[bloques];
    for(int i=0;i<bloques;i++)
    {
        temp[i] = bloque[permutacion[i]];
    }
    for(int i=0; i<bloques;i++)
    {
        bloque[i]=temp[i];
    }
 }

 void permutarinv(unsigned char bloque[])
 {
    unsigned char temp[bloques];
    for(int i=0;i<bloques;i++)
    {
        temp[i] = bloque[invpermutacion[i]];
    }
    for(int i=0; i<bloques;i++)
    {
        bloque[i]=temp[i];
    }
 }

 void cifrarbloque(unsigned char bloque[], char *clave)
{
    for(int j=0; j<rondas; j++)
    {
        clavexor(bloque,clave,j);
        sustituir(bloque);
        permutar(bloque);
    }
}

void descifrarbloque(unsigned char bloque[], char *clave)
{
    for(int j=rondas-1; j>=0; j--)
    {
        permutarinv(bloque);
        sustituirinv(bloque);
        clavexor(bloque,clave,j);
    }
}

int cifrararchivo(char *entrada, char *salida, char *clave)
{
    int fd1 = open(entrada, O_RDONLY);
    if(fd1 < 0)
    {
        perror("Error al abrir el archivo de entrada");
        return -1;
    }
    int fd2 = open(salida, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd2 < 0)
    {
        perror("Error al abrir el archivo de salida");
        close(fd1);
        return -1;
    }
    unsigned char bloque[bloques];
    int bytes;
    while((bytes = read(fd1,bloque,bloques))>0)
    {
        if(bytes<bloques)
        {
            int relleno = bloques - bytes;

            for(int i = bytes; i<bloques; i++)
            {
                bloque[i]=relleno;
            }
        }
        cifrarbloque(bloque,clave);
        write(fd2,bloque,bloques);
    }
    close(fd1);
    close(fd2);

    printf("Archivo cifrado exitosamente.\n");
    return 0;
}

int descifrararchivo(char *entrada, char *salida, char *clave)
{
    int fd1 = open(entrada, O_RDONLY);
    if(fd1 < 0)
    {
        perror("Error al abrir el archivo de entrada");
        return -1;
    }
    int fd2 = open(salida, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd2 < 0)
    {
        perror("Error al abrir el archivo de salida");
        close(fd1);
        return -1;
    }
    unsigned char bloque[bloques];
    int bytes;
    unsigned char ultimo[bloques];
    int tieneultimo = 0;
    while((bytes = read(fd1,bloque,bloques))==bloques)
    {
        if(tieneultimo)
        {
            descifrarbloque(ultimo, clave);
            write(fd2,ultimo,bloques);
        }
        memcpy(ultimo,bloque,bloques);
        tieneultimo = 1;
    }
    if(tieneultimo)
    {
        descifrarbloque(ultimo,clave);
        int relleno = ultimo[bloques-1];
        if(relleno > 0 && relleno <= bloques)
        {
            write(fd2,ultimo,bloques-relleno);
        }
        else
        {
            write(fd2,ultimo,bloques);
        }
    }
    close(fd1);
    close(fd2);
    printf("Archivo descifrado exitosamente.\n");
    return 0;
}

