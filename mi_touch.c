#include "directorios.h"
#include "bloques.h"

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "Uso: ./mi_touch <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    int permisos = atoi(argv[2]);
    if (permisos < 0 || permisos > 7) {
        fprintf(stderr, "Permisos incorrectos\n");
        return FALLO;
    }

    // Comprobar que NO es directorio
    if (argv[3][strlen(argv[3]) - 1] == '/') {
        fprintf(stderr, "Esto es un directorio, usa mkdir\n");
        return FALLO;
    }

    bmount(argv[1]);

    // Crea fichero
    if (mi_creat(argv[3], permisos) < 0) {
        fprintf(stderr, "Error al crear fichero\n");
        bumount();
        return FALLO;
    }

    bumount();
    return EXITO;
}