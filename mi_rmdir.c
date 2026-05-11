#include "directorios.h"
#include <stdio.h>
#include "ficheros_basico.h"

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Uso: ./mi_rmdir <disco> </ruta>\n");
        return FALLO;
    }

    if (strcmp(argv[2], "/") == 0) {
        fprintf(stderr, "Error: no se puede borrar la raiz\n");
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar disco\n");
        return FALLO;
    }

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo;
    unsigned int p_entrada;
    struct inodo inodo;

    int error = buscar_entrada(argv[2], &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if (error < 0) {
        fprintf(stderr, "Error: no existe\n");
        bumount();
        return FALLO;
    }

    leer_inodo(p_inodo, &inodo);

    // Solo directorios
    if (inodo.tipo != 'd') {
        fprintf(stderr, "Error: no es un directorio\n");
        bumount();
        return FALLO;
    }

    error = mi_unlink(argv[2]);

    if (error < 0) {
        fprintf(stderr, "Error al borrar\n");
        bumount();
        return FALLO;
    }

    bumount();
    return EXITO;
}