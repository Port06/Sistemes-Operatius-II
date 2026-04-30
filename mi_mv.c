#include "directorios.h"

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "Uso: ./mi_mv <disco> </origen> </destino/>\n");
        return FALLO;
    }

    // El destino debe ser directorio
    if (argv[3][strlen(argv[3]) - 1] != '/') {
        fprintf(stderr, "Error: destino debe ser un directorio\n");
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar disco\n");
        return FALLO;
    }

    // Extraer nombre del origen
    char inicial[TAMNOMBRE];
    char final[TAMNOMBRE * PROFUNDIDAD];
    char tipo;

    if (extraer_camino(argv[2], inicial, final, &tipo) == FALLO) {
        bumount();
        return FALLO;
    }

    // Construir nueva ruta: destino + nombre
    char nueva_ruta[TAMNOMBRE * PROFUNDIDAD];
    strcpy(nueva_ruta, argv[3]);
    strcat(nueva_ruta, inicial);

    // Si es directorio, mantener '/'
    if (tipo == 'd') {
        strcat(nueva_ruta, "/");
    }

    // Primero se crea el enlace en destino
    int error = mi_link(argv[2], nueva_ruta);
    if (error < 0) {
        fprintf(stderr, "Error al crear enlace (destino ya existe o fallo)\n");
        bumount();
        return FALLO;
    }

    // Luego se borra la entrada original
    error = mi_unlink(argv[2]);
    if (error < 0) {
        fprintf(stderr, "Error al eliminar origen\n");
        bumount();
        return FALLO;
    }

    bumount();
    return EXITO;
}