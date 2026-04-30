#include "directorios.h"

int main(int argc, char **argv) {

    // Comprobación básica de argumentos
    if (argc != 4) {
        fprintf(stderr, "Uso: ./mi_link <disco> </ruta1> </ruta2>\n");
        return FALLO;
    }

    // Montar disco
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar disco\n");
        return FALLO;
    }

    // Llamar a la función real
    int error = mi_link(argv[2], argv[3]);

    if (error < 0) {
        fprintf(stderr, "Error al crear enlace\n", error);
        bumount();
        return FALLO;
    }

    // Desmontar disco
    bumount();

    return EXITO;
}