#include "directorios.h"
#include "bloques.h"
#include <stdio.h>

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Uso: ./mi_stat <disco> </ruta>\n");
        return FALLO;
    }

    struct STAT stat;
    int p_inodo;
    bmount(argv[1]);

    // Obtener datos del inodo
    if ((p_inodo = mi_stat(argv[2], &stat)) < 0) {
        fprintf(stderr, "Error al obtener stat\n");
        bumount();
        return FALLO;
    }

    char formato[100];
    struct tm *tm;

    printf("no inodo %d\n", p_inodo);
    printf("tipo: %c\n", stat.tipo);
    printf("permisos: %d\n", stat.permisos);
    tm = localtime(&stat.atime);
    strftime(formato, sizeof(formato), "%a %Y-%m-%d %H:%M:%S", tm);
    printf("atime: %s\n", formato);
    tm = localtime(&stat.mtime);
    strftime(formato, sizeof(formato), "%a %Y-%m-%d %H:%M:%S", tm);
    printf("mtime: %s\n", formato);
    tm = localtime(&stat.ctime);
    strftime(formato, sizeof(formato), "%a %Y-%m-%d %H:%M:%S", tm);
    printf("ctime: %s\n", formato);
    printf("nlinks: %d\n", stat.nlinks);
    printf("tamEnBytesLog: %d\n", stat.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", stat.numBloquesOcupados);

    bumount();
    return EXITO;
}