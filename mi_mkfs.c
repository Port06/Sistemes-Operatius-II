//Archivo mi_mkfs.c

#include "bloques.h"

int main(int argc, char **argv) {

    if(bmount(argv[2]) == FALLO) {
        fprintf(stderr, RED "Error al montar el dispositivo" RESET);
    }

    for (int i=0; i<argv[1]; i++) {
        
    }
}