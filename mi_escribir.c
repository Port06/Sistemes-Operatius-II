#include "directorios.h"
#include "bloques.h"
#include <stdio.h>

int main(int argc, char **argv) {

    if (argc != 5) {
        fprintf(stderr, "Uso: ./mi_escribir <disco> </ruta> <texto> <offset>\n");
        return FALLO;
    }

    int offset = atoi(argv[4]);
    int l = strlen(argv[3]);
    

    fprintf(stderr, "longitud texto: %d \n", l);

    if(bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED "Error al montar el dispositivo\n" RESET);
        return FALLO;
    }


    // escribir texto en el fichero
    int escritos = mi_write(argv[2], argv[3], offset, strlen(argv[3]));

    if (escritos < 0) {
        fprintf(stderr, "Error al escribir\n");
        bumount();
        return FALLO;
    }

    printf("Bytes escritos: %d\n", escritos);

    if (bumount() == FALLO) {
        return FALLO;
    }
    return EXITO;
}