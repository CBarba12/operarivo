#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//funcion que determina que tipo de estadistica el usuario desea utilizar, devuelve un int segun la estadistica
int tipoPrograma(char* parametro){
int tipoOpe = -1;//en caso de parametro inesperado

if(strcmp(parametro, "CPU") == 0){
	tipoOpe = 1;
}else
if(strcmp(parametro, "Memoria") == 0){
	tipoOpe = 2;
}else
if(strcmp(parametro, "disco") == 0){
	tipoOpe = 3;
}

return tipoOpe;
}


int main(int argc, char** argv){

if(argc == 1){
printf("Error, no se han pasado los parametros suficientes\n");
return -1;
}

if(strcmp(argv[1], "?") == 0){
system("clear");
printf("Lista de parametros admitidos: \nCPU + 3456 \nMemoria + -r o -v\ndisco -tm o tg\n");
printf("\n\n\n\n\nPresione enter para continuar\n");
getchar();
return 0;
}

int tipoOpe = tipoPrograma(argv[1]);

int fd[2];//file del padre
if(pipe(fd) == -1){
printf("Error con los pipes\n");
return -2;
}

pid_t procc1 = fork();
if(procc1 == -1){
printf("ERROR EN LA CREACION DE UN SUBPROCCESO NECESARIO\n");
return -1;
}


if(procc1 == 0){
close(fd[0]);
dup2(fd[1], STDOUT_FILENO);//todo lo que se escriba en la salida estandar del hijo se escribe en el pipe
close(fd[1]);
	switch(tipoOpe){
		case 1:{
		execl("/usr/local/bin/CPU", "CPU", argv[2], NULL);
    	        }break;
		case 2:{
		execl("/usr/local/bin/Memoria", "Memoria", argv[2], argv[3], NULL);

		}break;
		case 3: {
		execl("/usr/local/bin/disco", "disco" , argv[2], NULL);
		}break;
		default:{//mensaje en caso de
		perror("El parametro escrito no es uno reconocido para el programa intentelo de nuevo o pruebe con ? para ayuda\n");
		return -1;
		}

	}
}else{
wait(NULL);
close(fd[1]);
dup2(fd[0], STDIN_FILENO);//leyendo de la entrada estandar
FILE* archivo = fdopen(fd[0], "r");

	switch(tipoOpe){
		case 1:{
		 char buffer[100];
                  while( fgets(buffer, sizeof(buffer), archivo) != NULL){
                  printf("%s", buffer);
                  }
		}break;
		case 2:{
		char nombre[100];
		float porcentaje = 0.0;
		int procId = 0;

		 while( fscanf(archivo, "%d%s porcentaje de memoria: %f\n", &procId, nombre, &porcentaje) == 3 ){
		 printf("%d  %s porcentaje de memoria: %f\n", procId, nombre, porcentaje);
		 }//while
		}break;
		case 3:{
		char buffer[100];
		  while( fgets(buffer, sizeof(buffer), archivo) != NULL){
		  printf("%s", buffer);
		  }
		}break;


	}//switch
fclose(archivo);
}//else

close(fd[0]);
return 0;
}
