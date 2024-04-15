#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
//verifica que una cadena es un numero valido para int
bool esNumero(const char* arg){

        for(int i = 0; arg[i] != '\0'; i++){
                if(!isdigit(arg[i])){
                return false;
                }
        }
return true;
}

//comienza calculo de tiempo de un proceso
// Función para obtener el tiempo de ejecución del proceso en segundos
float obtenerTiempoEjecucionProceso(int pid) {
    char ruta[256];
    snprintf(ruta, sizeof(ruta), "/proc/%d/stat", pid);

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        fprintf(stderr, "Error al abrir el archivo /proc/%d/stat\n", pid);
        return -1;
    }

    char linea[1024];  
    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        fprintf(stderr, "Error al leer el archivo /proc/%d/stat\n", pid);
        fclose(archivo);
        return -1;
    }

    fclose(archivo);

    char *token = strtok(linea, " "); 
    int contador = 1; 
    char nombre[256]; 

    unsigned long utime_ticks = 0, stime_ticks = 0;
    while (token != NULL) {
        if (contador == 2) { 
            strncpy(nombre, token, sizeof(nombre));
            nombre[sizeof(nombre) - 1] = '\0';  
        } else if (contador == 14) {  
            utime_ticks = strtoul(token, NULL, 10);
        } else if (contador == 15) {  
            stime_ticks = strtoul(token, NULL, 10);
            break;
        }
        token = strtok(NULL, " ");  
        contador++;  
    }

    unsigned long total_ticks = utime_ticks + stime_ticks;
    float tiempo_ejecucion_segundos = (float) total_ticks / sysconf(_SC_CLK_TCK);

    printf("Nombre del proceso: %s\n", nombre);
    return tiempo_ejecucion_segundos;
}


//optener el promedio de uso de cpu de un proceso
// Función para obtener el tiempo total de CPU durante los últimos 5 minutos
float obtenerTiempoTotalCPU() {
    FILE *archivo = fopen("/proc/stat", "r");
    if (archivo == NULL) {
        fprintf(stderr, "Error al abrir el archivo /proc/stat\n");
        return -1;
    }

    char linea[1024];
    unsigned long total_ticks = 0;

    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        fprintf(stderr, "Error al leer el archivo /proc/stat\n");
        fclose(archivo);
        return -1;
    }

    while (strncmp(linea, "cpu", 3) != 0) {
        if (fgets(linea, sizeof(linea), archivo) == NULL) {
            fprintf(stderr, "Error al leer el archivo /proc/stat\n");
            fclose(archivo);
            return -1;
        }
    }

    fclose(archivo);

    char *token = strtok(linea, " ");
    for (int i = 0; i < 10; i++) {
        token = strtok(NULL, " ");
        total_ticks += strtoul(token, NULL, 10);
    }

    return (float) total_ticks / sysconf(_SC_CLK_TCK);
}


//obtienes las estdadisticas del cpu del archivo /proc/stat 
//comienxa metodo para obtener para uso total de cpu
void getParametrosCPU(float** totalCPU, float** totalIdle){
FILE* archivo = fopen("/proc/stat", "r");
if(archivo == NULL){
printf("Fallo al abrir el archivo\n");
return;
}


char buffer[100];
float ticks_por_segundo = sysconf(_SC_CLK_TCK);//obtiene la cant de ticks de la maquina
float userTime = 0.0;
float niceTime = 0.0;
float systemTime = 0.0;
float idle = 0.0;
float iowait = 0.0;
float irq = 0.0;
float softirq = 0.0;
float steal = 0.0;

if( fgets(buffer, sizeof(buffer), archivo) != NULL){
sscanf(buffer, "%*s %f %f %f %f %f %f %f %f", &userTime, &niceTime, &systemTime, &idle, &iowait, &irq, &softirq, &steal);
*totalCPU = (float *)malloc(sizeof(float));
*totalIdle = (float *)malloc(sizeof(float));
**totalCPU = userTime + niceTime + systemTime + idle + irq + softirq + steal;//total del tiempo del cpu
**totalIdle = idle + iowait;//total del tiempo no trabajado por la cpu
}

//Convertir valores jiffies a segundos
**totalCPU = **totalCPU / ticks_por_segundo;
**totalIdle = **totalIdle / ticks_por_segundo;


fclose(archivo);
}//fin funcion


float cpu_Uso(){

float totalUsoCPU = 100.0;
float* cpuactivoPrev = NULL;
float* cpuinPrev = NULL;
float* cpuactivo = NULL;
float* cpuin = NULL;

//se toman 2 mediciones para comparar la carga de trabajo anterior(0 seg) con la actual(+1seg)
getParametrosCPU(&cpuactivoPrev, &cpuinPrev);
sleep(1);
getParametrosCPU(&cpuactivo, &cpuin);

if(cpuactivo != NULL){
float cpuAct = *cpuactivo - *cpuactivoPrev;//cpu actual activo
float cpuInc =  *cpuin - *cpuinPrev;//cpu actual inactivo idle + iowait
totalUsoCPU = totalUsoCPU - (( cpuInc / cpuAct ) * 100 );//calculo del % de idle, siendo total Uso de cpu = 100 - idel

free(cpuactivoPrev);
free(cpuinPrev);
free(cpuactivo);
free(cpuin);
}

return totalUsoCPU;
}

//argv[0] //program nombre, [1]posible pid
int main(int arc, char** argv){
if(argv[1] == NULL){//en caso no se especifique en pid se calcula el total de la cpu
 printf("Uso total de la cpu(usuario, sistema, interruciones etc...) : %f\n", cpu_Uso());
}
//en caso se especifique un pid
   if  (arc == 2){
     if(esNumero(argv[1]) == false){
	perror("El pid no es un numero valido\n");
	return -1;
     }
     int pid = atoi(argv[1]);

    float tiempo_ejecucion = obtenerTiempoEjecucionProceso(pid);
    if (tiempo_ejecucion >= 0) {
        float tiempo_total_cpu = obtenerTiempoTotalCPU();
        if (tiempo_total_cpu >= 0) {
            float porcentaje_utilizacion = (tiempo_ejecucion / tiempo_total_cpu) * 100;

            printf("PID: %d, Tiempo de utilización del proceso: %.2f\n", pid, tiempo_ejecucion); 
            printf("PID: %d, Porcentaje de utilización: %.2f%%\n", pid, porcentaje_utilizacion);
        } else {
            printf("Error al obtener el tiempo total de CPU\n");
        }
    } else {
        printf("Error al obtener el tiempo de ejecución del proceso con PID %d\n", pid);
    }

  }



return 0;
}
