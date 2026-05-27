#include "directorios.h"
#include "ficheros_basico.h"
#include "bloques.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Uso: ./mi_rn <disco> </ruta_antigua> <nuevo_nombre>\n");
        fprintf(stderr, "  <nuevo_nombre> no debe contener '/' ni exceder %d caracteres\n", TAMNOMBRE);
        return FALLO;
    }

    // Validar nuevo nombre
    if (strlen(argv[3]) == 0 || strlen(argv[3]) >= TAMNOMBRE) {
        fprintf(stderr, "Error: nombre no válido (longitud: %zu, máximo: %d)\n",
                strlen(argv[3]), TAMNOMBRE);
        return FALLO;
    }
    if (strchr(argv[3], '/') != NULL) {
        fprintf(stderr, "Error: el nuevo nombre no debe contener '/'\n");
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar disco\n");
        return FALLO;
    }

    // Detectar si es directorio (termina en '/')
    int es_directorio = 0;
    int len_ruta = strlen(argv[2]);
    if (len_ruta > 0 && argv[2][len_ruta - 1] == '/') {
        es_directorio = 1;
    }

    // Buscar entrada original
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error = buscar_entrada(argv[2], &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if (error < 0) {
        fprintf(stderr, "Error: no existe la ruta %s\n", argv[2]);
        bumount();
        return FALLO;
    }

    // Obtener el directorio padre
    char dir_padre[1024];
    const char *ultima_barra = strrchr(argv[2], '/');
    if (ultima_barra == NULL) {
        fprintf(stderr, "Error: ruta inválida\n");
        bumount();
        return FALLO;
    }
    size_t len_dir = ultima_barra - argv[2];
    if (len_dir == 0) {
        strcpy(dir_padre, "/");
    } else {
        strncpy(dir_padre, argv[2], len_dir);
        dir_padre[len_dir] = '\0';
    }

    // Verificar que el nuevo nombre no existe en el directorio padre
    char camino_nuevo[1024];
    if (strcmp(dir_padre, "/") == 0)
        sprintf(camino_nuevo, "/%s", argv[3]);
    else
        sprintf(camino_nuevo, "%s/%s", dir_padre, argv[3]);
    if (es_directorio) strcat(camino_nuevo, "/");

    unsigned int p_inodo_dir_temp = 0, p_inodo_temp, p_entrada_temp;
    error = buscar_entrada(camino_nuevo, &p_inodo_dir_temp, &p_inodo_temp, &p_entrada_temp, 0, 0);
    if (error != ERROR_NO_EXISTE_ENTRADA_CONSULTA && error >= 0) {
        fprintf(stderr, "Error: ya existe un elemento con nombre '%s'\n", argv[3]);
        bumount();
        return FALLO;
    }

    // Leer la entrada actual
    struct entrada ent;
    if (mi_read_f(p_inodo_dir, &ent, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) != sizeof(struct entrada)) {
        fprintf(stderr, "Error al leer entrada\n");
        bumount();
        return FALLO;
    }

    // Cambiar nombre
    strncpy(ent.nombre, argv[3], TAMNOMBRE - 1);
    ent.nombre[TAMNOMBRE - 1] = '\0';

    // Escribir entrada modificada
    if (mi_write_f(p_inodo_dir, &ent, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) != sizeof(struct entrada)) {
        fprintf(stderr, "Error al escribir entrada\n");
        bumount();
        return FALLO;
    }

    // Actualizar CTIME del directorio padre
    struct inodo inodo_dir_padre;
    if (leer_inodo(p_inodo_dir, &inodo_dir_padre) == FALLO) {
        fprintf(stderr, "Error al leer inodo del directorio padre\n");
        bumount();
        return FALLO;
    }
    inodo_dir_padre.ctime = time(NULL);
    if (escribir_inodo(p_inodo_dir, &inodo_dir_padre) == FALLO) {
        fprintf(stderr, "Error al actualizar ctime\n");
        bumount();
        return FALLO;
    }

    printf("Renombrado exitoso: %s -> %s\n", argv[2], camino_nuevo);
    bumount();
    return EXITO;
}