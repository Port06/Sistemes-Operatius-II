#include "directorios.h"
#include "bloques.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr,
                "Uso: ./mi_mv <disco> </origen> </destino/>\n");
        return FALLO;
    }

    // El destino debe acabar en '/'
    if (argv[3][strlen(argv[3]) - 1] != '/') {
        fprintf(stderr,
                "Error: el destino debe ser un directorio\n");
        return FALLO;
    }

    // Montar disco
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr,
                "Error al montar el disco\n");
        return FALLO;
    }

    // Detectar si origen es fichero o directorio
    char tipo;

    int len = strlen(argv[2]);

    if (argv[2][len - 1] == '/') {
        tipo = 'd';
    } else {
        tipo = 'f';
    }

    // No permitimos mover directorios
    if (tipo == 'd') {
        fprintf(stderr,
                "Error: no se pueden mover directorios\n");
        bumount();
        return FALLO;
    }

    // Obtener nombre final del fichero
    char *nombre = strrchr(argv[2], '/');

    if (nombre == NULL || strlen(nombre) <= 1) {
        fprintf(stderr,
                "Error al obtener el nombre del fichero\n");
        bumount();
        return FALLO;
    }

    nombre++; // saltar '/'

    // Construir nueva ruta
    char nueva_ruta[1024];

    strcpy(nueva_ruta, argv[3]);
    strcat(nueva_ruta, nombre);

    // Crear enlace en destino
    int error = mi_link(argv[2], nueva_ruta);

    if (error < 0) {
        fprintf(stderr,
                "Error al crear enlace en destino\n");
        bumount();
        return FALLO;
    }

    // Eliminar entrada original
    error = mi_unlink(argv[2]);

    if (error < 0) {
        fprintf(stderr,
                "Error al eliminar origen\n");
        bumount();
        return FALLO;
    }

    // Desmontar disco
    bumount();

    return EXITO;
}