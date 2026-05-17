#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define max_entrada 1024
#define max_arg 64

void ComEntrada(char *entrada){
	printf("\nsina>");
	fflush(stdout);
	if(fgets(entrada,max_entrada,stdin)==NULL){
		exit(0);
		
	}
	entrada[strcspn(entrada, "\n")]=0;  //encuentra el salto de linea y lo elimina


}

int main(){
	
	char entrada[max_entrada];
	char *arg[max_arg];
	while(1){
		ComEntrada(entrada);
		if(strlen(entrada)==0)
			continue;
		
	}

}
