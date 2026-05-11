#include "directorios.h"
#include "bloques.h"
#include <stdio.h>

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Uso: ./mi_stat <disco> </ruta>\n");
        return FALLO;
    }

    struct STAT stat;

    bmount(argv[1]);

    // Obtener datos del inodo
    if (mi_stat(argv[2], &stat) < 0) {
        fprintf(stderr, "Error al obtener stat\n");
        bumount();
        return FALLO;
    }

    // Imprimir info básica
    printf("tipo: %c\n", stat.tipo);
    printf("permisos: %d\n", stat.permisos);
    printf("atime: %ld\n", stat.atime);
    printf("mtime: %ld\n", stat.mtime);
    printf("ctime: %ld\n", stat.ctime);
    printf("nlinks: %d\n", stat.nlinks);
    printf("tamEnBytesLog: %d\n", stat.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", stat.numBloquesOcupados);

    bumount();
    return EXITO;
}