#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define max_entrada 1024
#define max_arg 64
#define max_cmd

void ComEntrada(char *entrada){
	printf("\nsina>");
	fflush(stdout);
	if(fgets(entrada,max_entrada,stdin)==NULL){
		exit(0);
		
	}
	entrada[strcspn(entrada, "\n")]=0;  //encuentra el salto de linea y lo elimina


}

void ComandoPipe(char *entrada)
{
	char *comando[max_cmd];
	int cantcomandos = 0;
	int pipes[max_cmd-1][2];
	char *arg[max_arg];
	comandos[cantcomandos] =  strtok(entrada, "|");
	while(comandos[cantcomandos]!=NULL && comandos[cantcomandos]!='\0')
	{
		cantcomandos++;
		comandos[cantcomandos] = strtok(NULL,"|");
	}
	for(int i = 0; i<cantcomandos-1;i++)
	{
		if(pipe(pipes[i])<0)
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
			 
		}
	}
	
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
		execvp(arg[0],arg);
		perror("Error al ejecutar el comando");
		exit(1);
	}
	else
	{
		wait(NULL);
	}
}

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
		if(strcmp(arg[0],"exit")==0)
	   	  	break;

		if(strcmp(arg[0],"cd")==0)
		{
			if(arg[1]==NULL)
			{
				printf("Falta agregar la ruta\n");
			}
			else
			{
				chdir(arg[1]);
				continue;
			}
		}
		EjecutarComando(arg);		
	}
	return 0;
}
