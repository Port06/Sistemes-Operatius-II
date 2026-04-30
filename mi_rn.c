#include "directorios.h"

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "Uso: ./mi_rn <disco> </ruta_antiguo> <nuevo>\n");
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar disco\n");
        return FALLO;
    }

    char inicial[TAMNOMBRE];
    char final[TAMNOMBRE * PROFUNDIDAD];
    char tipo;

    // Separar ruta en (directorio padre + nombre antiguo)
    if (extraer_camino(argv[2], inicial, final, &tipo) == FALLO) {
        bumount();
        return FALLO;
    }

    // Obtener directorio padre
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo;
    unsigned int p_entrada;

    int error = buscar_entrada(argv[2], &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if (error < 0) {
        fprintf(stderr, "Error: no existe la ruta\n");
        bumount();
        return FALLO;
    }

    // Leer entrada original
    struct entrada entrada;

    if (mi_read_f(p_inodo_dir, &entrada, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) < 0) {
        fprintf(stderr, "Error al leer entrada\n");
        bumount();
        return FALLO;
    }

    // Comprobar que no exista el nuevo nombre en el mismo directorio
    unsigned int p_inodo_aux, p_entrada_aux;
    if (buscar_entrada(final, &p_inodo_dir, &p_inodo_aux, &p_entrada_aux, 0, 0) == EXITO) {
        fprintf(stderr, "Error: ya existe una entrada con ese nombre\n");
        bumount();
        return FALLO;
    }

    // Cambiar nombre
    strncpy(entrada.nombre, argv[3], TAMNOMBRE);
    entrada.nombre[TAMNOMBRE - 1] = '\0';

    // Escribir entrada modificada
    if (mi_write_f(p_inodo_dir, &entrada, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) < 0) {
        fprintf(stderr, "Error al escribir entrada\n");
        bumount();
        return FALLO;
    }

    bumount();
    return EXITO;
}