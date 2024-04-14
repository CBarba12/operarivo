#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//verifica que la cadena pasada sea un numero
bool esNumero(const char* arg){

        for(int i = 0; arg[i] != '\0'; i++){
                if(!isdigit(arg[i])){
                return false;
                }
        }
return true;
}

//obtiene la cantidad de bytes usados por los procesos del sistema
float memUsada(int pid){
float bytes = 0;
char buffer[100];
char rutaVariable[22];

//sirve para obtener info de status de un x proceso, x se pasa por parametro
sprintf(rutaVariable, "/proc/%d/status", pid);

FILE* archivo = NULL;
archivo = fopen(rutaVariable, "r");

while(fgets(buffer, sizeof(buffer), archivo) != NULL){
	if( strstr(buffer,  "VmSize:") != NULL ){
	sscanf(buffer,  "VmSize: %f kB", &bytes);
	break;
	}
}

fclose(archivo);
return bytes;
}

//obtiene la cantidad de memorial real(fisica del equipo)
float cantidadMemF(){
char buffer[35];
float cantMem = 0;

FILE* archivo = NULL;
//se consulta el archivo /proc/meminfo del sistema para saber la cant real de ram instalada en el equipo
archivo = fopen("/proc/meminfo", "r");

while( fgets(buffer, sizeof(buffer), archivo) != NULL ){
	if( strstr(buffer, "MemTotal:") != NULL ){
	sscanf(buffer, "MemTotal: %f kB", &cantMem);
	}
	break;
}

fclose(archivo);
return cantMem;
}


//obtiene la cantidad de bytes de memoria ocupada por un proceso y el nombre
float cantidadMem(int pid, char* palabraClave, char* palabraExtr, char** nombreF){
char rutaVariable[22];
char nombreProc[25];
char buffer[100];
float cantBytes = 0;

sprintf(rutaVariable, "/proc/%d/status", pid);
FILE* archivo = fopen(rutaVariable, "r");

if(archivo == NULL){
return -1;
}

while(fgets(buffer, sizeof(buffer), archivo) != NULL){
	if(strstr(buffer, "Name:") != NULL){
	sscanf(buffer, "Name: %s Umask", nombreProc);
	break;
	}
}

*nombreF = (char*)malloc( (strlen(nombreProc) + 1) * sizeof(char));
strcpy(*nombreF, nombreProc);

while(fgets(buffer, sizeof(buffer), archivo) != NULL){
        if(strstr(buffer, palabraClave) != NULL){
        sscanf(buffer, palabraExtr, &cantBytes);
        break;
        }
}

fclose(archivo);
return cantBytes;
}


//argv 0 = nombre del programa a ejecutar, 1 = parametro -v o -r, 2 = pid(opcional)
int main(int argc, char** argv){
float bytesTotal = 0;
int tipoMem = 0;
char* palabraClave = NULL;
char* palabraExtracc = NULL;


if(strcmp(argv[1], "-v") == 0){//caso memoria virtual
tipoMem = 1;
palabraClave = "VmSize:";
palabraExtracc = "VmSize: %f kB";

}else
if(strcmp(argv[1], "-r") == 0){//caso memoria real(fisica)
tipoMem = 2;
palabraClave = "VmRSS:";
palabraExtracc = "VmRSS: %f kB";
}

if(tipoMem == 1){
	//abriendo el fichero /proc para recolectar el total de bytes de memoria virtual usada
        //el % de memoria virtual se calcula dividiendo el % de memoria de x proceso entre la sum total de todos los proc del sist
	DIR* fhichero = NULL;
	fhichero = opendir("/proc");
	if(opendir == NULL){
	return -1;
	}

	struct dirent *lector;
	while( (lector = readdir(fhichero)) != NULL){
		if(esNumero(lector->d_name) ){
		bytesTotal = bytesTotal + memUsada(atoi(lector->d_name));
		}
	}
	closedir(fhichero);

}else{//el tipo de memoria es real
	bytesTotal = cantidadMemF();
}

//abriendo el fichero de los procesos para obtener el total de bytes de cada proceso y sus nombre

DIR* fichero2 = NULL;
fichero2 = opendir("/proc");
if(fichero2 == NULL){
return -1;
}


float bytes = 0;
char* nomProc = NULL;
struct dirent* lector2;
float porcBytes = 0;
char porcBytes2[10];

if(argv[2] != NULL){//caso donde se obtiene el porcentaje para un proceso en especifico especificado en argv[2]
	if( esNumero(argv[2]) ){
	  while( (lector2 = readdir(fichero2)) != NULL){
	  if(strcmp(argv[2], lector2->d_name) == 0){
	  bytes = cantidadMem(atoi(lector2->d_name), palabraClave, palabraExtracc, &nomProc);
          printf("%s %s porcentaje de memoria: ", lector2->d_name, nomProc);
          printf(" %f\n", (bytes/bytesTotal) * 100);
          free(nomProc);
	  }
	}
	}

}else{//caso donde se obtienen los porcentajes de todos los procesos

	while( (lector2 = readdir(fichero2)) != NULL){
		if(esNumero(lector2->d_name) ){
		bytes = cantidadMem(atoi(lector2->d_name), palabraClave, palabraExtracc, &nomProc);
		printf("%s %s porcentaje de memoria: ", lector2->d_name, nomProc);
		printf(" %f\n", (bytes/bytesTotal) * 100);
		free(nomProc);
		}
	}
}


closedir(fichero2);
return 0;

}
