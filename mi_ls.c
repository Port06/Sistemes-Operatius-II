#include "directorios.h"

int main(int argc, char **argv) {

    char buffer[4096 * 10]; // Bsuffer grande para resultados
    memset(buffer, 0, sizeof(buffer));

    int flag_l = 0;
    char *camino;

    // Detectar si hay -l
    if (argc == 4 && strcmp(argv[1], "-l") == 0) {
        flag_l = 1;
        camino = argv[3];
    } else if (argc == 3) {
        camino = argv[2];
    } else {
        fprintf(stderr, "Uso: ./mi_ls [-l] <disco> </ruta>\n");
        return FALLO;
    }

    bmount(argv[flag_l ? 2 : 1]);

    // Pedir listado
    int n = mi_dir(camino, buffer, flag_l);

    if (n < 0) {
        fprintf(stderr, "Error al listar\n");
        bumount();
        return FALLO;
    }

    // Imprimir resultado
    printf("Total: %d\n%s\n", n, buffer);

    bumount();
    return EXITO;
}