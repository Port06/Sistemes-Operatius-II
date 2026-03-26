#include <stdio.h>
#include <stdlib.h>
#include "ficheros.h"
#include "ficheros_basico.h"

int main(int argc, char **argv) {

    // Validación de sintaxis
    if (argc != 4) {
        fprintf(stderr, "Sintaxis: ./permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return -1;
    }

    // Montar dispositivo
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error al montar el dispositivo\n");
        return -1;
    }

    // Convertir argumentos
    unsigned int ninodo = atoi(argv[2]);
    unsigned char permisos = atoi(argv[3]);

    // Llamar a mi_chmod_f
    if (mi_chmod_f(ninodo, permisos) == -1) {
        fprintf(stderr, "Error al cambiar permisos\n");
    }

    // Desmontar dispositivo
    bumount();

    return 0;
}