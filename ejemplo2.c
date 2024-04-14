#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <PID>\n", argv[0]);
        return 1;
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

    return 0;
}












