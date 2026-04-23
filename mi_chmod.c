#include "directorios.h"

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "Uso: ./mi_chmod <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    int permisos = atoi(argv[2]);
    if (permisos < 0 || permisos > 7) {
        fprintf(stderr, "Permisos incorrectos\n");
        return FALLO;
    }

    bmount(argv[1]);

    // Cambiar permisos
    if (mi_chmod(argv[3], permisos) < 0) {
        fprintf(stderr, "Error al cambiar permisos\n");
        bumount();
        return FALLO;
    }

    bumount();
    return EXITO;
}