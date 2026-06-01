#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> 

#include "bitacora.h"
#include "mcensuran.h"
#include "encriptar.h"

#define max_entrada 1024
#define max_arg 64
#define max_cmd 20


void ComEntrada(char *entrada);

void SepararComando(char *entrada, char *arg[]);

int ComandoInternoprincipal(char *arg[]);

int ComandoInternoEnHijo(char *arg[]);

void EjecutarComando(char *arg[]);

void ComandoPipe(char *entrada);

void redireccionamiento(char *arg[]);

int main(){
	
	char entrada[max_entrada];
	char *arg[max_arg];
	while(1){
		ComEntrada(entrada);
		if(strlen(entrada)==0)
			continue;

		if(strchr(entrada,'|')!=NULL)
		{
			ComandoPipe(entrada);
			continue;
		}

        SepararComando(entrada,arg);
		if(ComandoInternoprincipal(arg))
			continue;

		EjecutarComando(arg);		
	}
	return 0;
}


void ComEntrada(char *entrada){
	printf("\nsina>");
	fflush(stdout);
	if(fgets(entrada,max_entrada,stdin)==NULL){
		exit(0);
		
	}
	entrada[strcspn(entrada, "\n")]=0;  //encuentra el salto de linea y lo elimina


}
void  SepararComando(char *entrada, char *arg[])
{
	int i=0, j=0; //i es la cadena, j la palabra
	while(entrada[i]!='\0')
	{
		while(entrada[i] ==' ')
		{
			i++;
		}
		if(entrada[i]=='\0')
		{
			break;
		}
		if(entrada[i]=='"')
		{
			i++;
			arg[j] = &entrada[i];
			while(entrada[i]!='"'&& entrada[i]!='\0')
			{
				i++;
				
			}
			entrada[i]='\0';
			i++;
			j++;
		}
		else
		{
			arg[j]=&entrada[i];
			while(entrada[i]!=' ' && entrada[i] != '\0')
			{
				i++;
			}
			if(entrada[i]!='\0')
			{
				entrada[i] = '\0';
				i++;
			}
			j++;
		}
	}
	arg[j]= NULL;
}

int ComandoInternoprincipal(char *arg[])
{
	if(arg[0] == NULL)
    	return 0;
    if(strcmp(arg[0], "exit") == 0)
    {
        exit(0);
    }

    if(strcmp(arg[0], "cd") == 0)
    {
        if(arg[1]==NULL)
			{
				char *home = getenv("HOME");
				if(home!=NULL)
				{
					chdir(home);
					
				}
			}
			else
			{
				chdir(arg[1]);
				
			}

        return 1;
    }
	


    return 0;
}

int ComandoInternoEnHijo(char *arg[])
{
	if(arg[0] == NULL)
		return 0;
	if(strcmp(arg[0], "set") == 0)
		{
			printf("HOME=%s\n", getenv("HOME"));
			printf("PATH=%s\n", getenv("PATH"));
			printf("USER=%s\n", getenv("USER"));
			return 1;
		}
    if(strcmp(arg[0], "help") == 0)
    {
        printf("\n=== Ayuda de SINA Shell ===\n");
		printf("Comandos internos:\n");
		printf("  cd [ruta]                         Cambia de directorio\n");
		printf("  exit                              Sale del shell\n");
		printf("  help                              Muestra esta ayuda\n");
		printf("  set                               Muestra variables HOME, PATH y USER\n");
		printf("  bitacora <archivo> <mensaje>      Agrega un registro a la bitacora\n");
		printf("  mcensuran <archivo> <lista>       Censura palabras usando una lista\n");
		printf("  encriptar -e <in> <out> <clave>   Cifra un archivo\n");
		printf("  encriptar -d <in> <out> <clave>   Descifra un archivo\n");
		printf("  ./ocultarinfo.sh <archivo>                  Oculta correos, claves, tokens e IPs\n");
		printf("  ./consultarbitacora.sh <bitacora> [filtro] [valor]  Consulta la bitacora\n");

		printf("\nComandos externos:\n");
		printf("  Puedes usar comandos de Linux como ls, cat, grep, mkdir, rm, touch, etc.\n");

		printf("\nFunciones soportadas:\n");
		printf("  Pipes:              comando1 | comando2 | comando3\n");
		printf("  Redireccion salida: comando > archivo\n");
		printf("  Append salida:      comando >> archivo\n");
		printf("  Redireccion entrada: comando < archivo\n");

		printf("\nEjemplos:\n");
		printf("  ls -l\n");
		printf("  cat sibna.c | grep include\n");
		printf("  cat sibna.c | grep include | wc -l\n");
		printf("  ls > salida.txt\n");
		printf("  sort < nombres.txt > ordenados.txt\n");
		printf("  bitacora cambios.txt \"se agrego una funcion\"\n");
		printf("  mcensuran prueba.txt listainfantil.txt\n");
		printf("  encriptar -e prueba.txt prueba.enc clave123\n");
		printf("  encriptar -d prueba.enc recuperado.txt clave123\n");


        return 1;
    }
	if(strcmp(arg[0], "bitacora") == 0)
	{
		bitacora(arg);
		return 1;
	}
	if(strcmp(arg[0],"mcensuran")==0)
	{
		mcensuran(arg);
		return 1;
	}
	if(strcmp(arg[0],"encriptar")==0)
	{
		encriptar(arg);
		return 1;
	}
	return 0;
}
void redireccionamiento(char *arg[])
{
	int i= 0, j=0;
	int entrada=0, salida=0;
	char *argtemp[max_arg];

	while(arg[i]!=NULL)
	{
		if(strcmp(arg[i],"<")==0)
		{
			if(entrada!=0)
			{
				printf("Error: Solo se perimite un redireccionamiento de entrada\n");
				return;
			}
			if(arg[i+1]==NULL)
			{
				printf("Error: Falta el archivo de entrada\n");
				return;
			}
			entrada=1;
			int fd = open(arg[i+1], O_RDONLY);
			if(fd < 0)
			{
				perror("Error al abrir");
				return;
			}
			dup2(fd,0);
			close(fd);
			i+=2;
		}
		else if(strcmp(arg[i],">")==0)
		{
			if(salida!=0)
			{
				printf("Error: Solo se perimite un redireccionamiento de salida\n");
				return;
			}
			if(arg[i+1]==NULL)
			{
				printf("Error: Falta el archivo de salida\n");
				return;
			}
			salida=1;
			int fd = open(arg[i+1],O_WRONLY|O_CREAT|O_TRUNC,0644); 
			if(fd < 0)
			{
				perror("open");
				return;
			}
			dup2(fd,1);
			close(fd);
			i+=2;
		}
		else if(strcmp(arg[i],">>")==0)
		{
			if(salida!=0)
			{
				printf("Error: Solo se perimite un redireccionamiento de salida\n");
				return;
			}
			if(arg[i+1]==NULL)
			{
				printf("Error: Falta el archivo de salida\n");
				return;
			}
			salida=1;
			int fd = open(arg[i+1],O_WRONLY|O_CREAT|O_APPEND,0644);
			if(fd < 0)
			{
				perror("open");
				return;
			}
			dup2(fd,1);
			close(fd);
			i+=2;


		}
		else
		{
			argtemp[j]=arg[i];
			i++;
			j++;
		}
		
	}
	argtemp[j]=NULL;

	for(i=0;argtemp[i]!=NULL;i++)
	{
		arg[i]=argtemp[i];
	}
	arg[i]=NULL;
	
}
void ComandoPipe(char *entrada)
{
	char *comandos[max_cmd];
	int cantcomandos = 0;
	int pipefds[max_cmd-1][2];
	char *arg[max_arg];
	comandos[cantcomandos] =  strtok(entrada, "|");
	while(comandos[cantcomandos]!=NULL && cantcomandos < max_cmd-1)
	{
		cantcomandos++;
		comandos[cantcomandos] = strtok(NULL,"|");
	}
	for(int i = 0; i<cantcomandos-1;i++)
	{
		if(pipe(pipefds[i])<0)
		{
			perror("Error al crear el pipe");
			return;
		}
	}
	for(int i = 0; i<cantcomandos;i++)
	{
		pid_t pid = fork();
		if(pid<0)
		{
			perror("Error al crear el procesos");
			return;
		}
		if(pid==0)
		{
			if(i>0)
			{
				dup2(pipefds[i-1][0],0);
			} 
			if(i<cantcomandos-1)
			{
				dup2(pipefds[i][1],1);
			}
			for(int j=0; j<cantcomandos-1;j++)
			{
				close(pipefds[j][0]);
				close(pipefds[j][1]);
			}
			   SepararComando(comandos[i], arg);
			   redireccionamiento(arg);
			   if(ComandoInternoEnHijo(arg))
					exit(0);
          		execvp(arg[0], arg);
           		perror("Error al ejecutar comando");
            	exit(1); 
		}
	}
	for(int i=0; i<cantcomandos-1;i++)
	{
		close(pipefds[i][0]);
		close(pipefds[i][1]);
	}
	for(int i=0; i<cantcomandos;i++)
	{
		wait(NULL);
	}
	
}



void EjecutarComando(char *arg[])
{
	
	pid_t pid=fork();
	
	if(pid<0)
	{
		perror("Eror al crear el proceso");
		return;
	}
	if(pid == 0)
	{
		redireccionamiento(arg);
		if(ComandoInternoEnHijo(arg))
   			 exit(0);
        execvp(arg[0], arg);
    	perror("Error al ejecutar comando");
       	exit(1);		
	}
	else
	{
		wait(NULL);
	}
}

