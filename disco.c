#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 

void mostrarEspacioDisco(long total, long usado, long libre, int m) {
    double total_tamanio, total_usado, total_libre;
    double porcentajeDeuso, porcentajeLibre;

    switch (m) {
        case 1:
            total_tamanio = total / (1024.0);
            total_usado = usado / (1024.0);
            total_libre = libre / (1024.0);

            printf("----------------------------------Datos del Disco-----------------------------------\n");
            printf("Espacio total en GB: %.2f\n", total_tamanio);
            printf("Espacio utilizado en GB: %.2f\n", total_usado);
            printf("Espacio libre en GB: %.2f\n", total_libre);

            porcentajeDeuso = (usado / (double)total) * 100.0;
            porcentajeLibre = (libre / (double)total) * 100.0;

            printf("----------------------------------Porcentaje del Disco-----------------------------------\n");
            printf("Porcentaje de espacio utilizado: %.2f%%\n", porcentajeDeuso);
            printf("Porcentaje de espacio Libre: %.2f%%\n", porcentajeLibre);
            printf("\n");

            break;
        case 2:
            total_tamanio = total;
            total_usado = usado;
            total_libre = libre;

            printf("----------------------------------Datos del Disco-----------------------------------\n");
            printf("Espacio total en MB: %.2f\n", total_tamanio);
            printf("Espacio utilizado en MB: %.2f\n", total_usado);
            printf("Espacio libre en MB: %.2f\n", total_libre);

            porcentajeDeuso = (usado / (double)total) * 100.0;
            porcentajeLibre = (libre / (double)total) * 100.0;

            printf("----------------------------------Porcentaje del Disco-----------------------------------\n");
            printf("Porcentaje de espacio utilizado: %.2f%%\n", porcentajeDeuso);
            printf("Porcentaje de espacio Libre: %.2f%%\n", porcentajeLibre);
            printf("\n");

            break;
        case 3:
            total_tamanio = total * 1024.0;
            total_usado = usado * 1024.0;
            total_libre = libre * 1024.0;

            printf("----------------------------------Datos del Disco-----------------------------------\n");
            printf("Espacio total en KB: %.2f\n", total_tamanio);
            printf("Espacio utilizado en KB: %.2f\n", total_usado);
            printf("Espacio libre en KB: %.2f\n", total_libre);

            porcentajeDeuso = (usado / (double)total) * 100.0;
            porcentajeLibre = (libre / (double)total) * 100.0;

            printf("----------------------------------Porcentaje del Disco-----------------------------------\n");
            printf("Porcentaje de espacio utilizado: %.2f%%\n", porcentajeDeuso);
            printf("Porcentaje de espacio Libre: %.2f%%\n", porcentajeLibre);
            printf("\n");

            break;
        default:
            printf("Opción no válida.\n");
            return;
    }
}

int obtenerDatosDisco(int unidad) {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 

    FILE *archivo = popen("df -BM /", "r");
    if (archivo == NULL) {
        perror("Error al abrir df ");
        return 1;
    }

    char vector[1024];
    while (fgets(vector, sizeof(vector), archivo) != NULL) {
        if (strstr(vector, "/") != NULL) {
            unsigned long total, usado, libre;
            if (sscanf(vector, "%*s %luM %luM %luM", &total, &usado, &libre) == 3) {
                mostrarEspacioDisco(total, usado, libre, unidad);
            } else {
                printf("Error al leer la línea del comando df\n");
            }
            break;
        }
    }

    pclose(archivo);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <unidad>\n", argv[0]);
        printf("Unidad puede ser -tg para GB, -tm para MB, o -tk para KB\n");
        return 1;
    }

    int variable;
    char *unidad = argv[1];

    if (strlen(unidad) != 3 || unidad[0] != '-') {
        printf("Formato de unidad incorrecto. Utilice -tg para GB, -tm para MB, o -tk para KB\n");
        return 1;
    }

    switch (unidad[1]) {
        case 't':
            switch (unidad[2]) {
                case 'g':
                    variable = 1; 
                    break;
                case 'm':
                    variable = 2; 
                    break;
                case 'k':
                    variable = 3; 
                    break;
                default:
                    printf("Unidad no válida. Utilice -tg para GB, -tm para MB, o -tk para KB\n");
                    return 1;
            }
            break;
        default:
             printf("Unidad no válida. Utilice -tg para GB, -tm para MB, o -tk para KB\n");
            return 1;
    }

    obtenerDatosDisco(variable);
    return 0;
}