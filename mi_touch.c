#include "directorios.h"
#include "bloques.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "Uso: ./mi_touch <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    // Validar permisos
    int permisos = atoi(argv[2]);

    if (permisos < 0 || permisos > 7) {
        fprintf(stderr, "Permisos incorrectos (0-7)\n");
        return FALLO;
    }

    // Comprobar que NO acaba en '/'
    // touch solo crea ficheros
    if (argv[3][strlen(argv[3]) - 1] == '/') {
        fprintf(stderr, "Error: la ruta corresponde a un directorio\n");
        return FALLO;
    }

    // Montar dispositivo
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar el dispositivo\n");
        return FALLO;
    }

    // Crear fichero
    if (mi_creat(argv[3], permisos) == FALLO) {
        fprintf(stderr, "Error al crear fichero\n");
        bumount();
        return FALLO;
    }

    // Desmontar dispositivo
    bumount();

    return EXITO;
}