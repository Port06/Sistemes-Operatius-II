#include "directorios.h"
#include "bloques.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr,
                "Uso: ./mi_rn <disco> </ruta_antigua> <nuevo_nombre>\n");
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar disco\n");
        return FALLO;
    }

    char inicial[TAMNOMBRE];
    char final[256];
    char tipo;

    // Separar camino
    if (extraer_camino(argv[2],
                       inicial,
                       final,
                       &tipo) == FALLO) {

        fprintf(stderr, "Error al extraer camino\n");
        bumount();
        return FALLO;
    }

    // Buscar entrada original
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    int error = buscar_entrada(argv[2],
                               &p_inodo_dir,
                               &p_inodo,
                               &p_entrada,
                               0,
                               0);

    if (error < 0) {
        fprintf(stderr, "Error: no existe la ruta\n");
        bumount();
        return FALLO;
    }

    // Leer entrada
    struct entrada ent;

    if (mi_read_f(p_inodo_dir,
                  &ent,
                  p_entrada * sizeof(struct entrada),
                  sizeof(struct entrada)) < 0) {

        fprintf(stderr, "Error al leer entrada\n");
        bumount();
        return FALLO;
    }

    // Cambiar nombre
    strncpy(ent.nombre, argv[3], TAMNOMBRE);

    ent.nombre[TAMNOMBRE - 1] = '\0';

    // Escribir entrada modificada
    if (mi_write_f(p_inodo_dir,
                   &ent,
                   p_entrada * sizeof(struct entrada),
                   sizeof(struct entrada)) < 0) {

        fprintf(stderr, "Error al escribir entrada\n");
        bumount();
        return FALLO;
    }

    bumount();

    return EXITO;
}