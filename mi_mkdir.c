#include "directorios.h"
#include "bloques.h"
#include <stdio.h>


int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "Uso: ./mi_mkdir <disco> <permisos> </ruta/>\n");
        return FALLO;
    }

    // Comprobar permisos válidos
    int permisos = atoi(argv[2]);
    if (permisos < 0 || permisos > 7) {
        fprintf(stderr, "Permisos incorrectos (0-7)\n");
        return FALLO;
    }

    // Comprobar que es directorio (acaba en '/')
    if (argv[3][strlen(argv[3]) - 1] != '/') {
        fprintf(stderr, "La ruta debe acabar en '/'\n");
        return FALLO;
    }

    bmount(argv[1]);

    // Crea el directorio
    if (mi_creat(argv[3], permisos) < 0) {
        fprintf(stderr, "Error al crear directorio\n");
        bumount();
        return FALLO;
    }

    bumount();
    return EXITO;
}