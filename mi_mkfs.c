//Archivo mi_mkfs.c

#include "bloques.h"
#include "ficheros_basico.h"

int main(int argc, char **argv) {

    //Comprobamos si el numero de datos pasado por paramentro son igual a 3
    if (argc != 3) {
        fprintf(stderr, RED "Error al pasar los argumentos\n" RESET);
    }

    //inicializacion de Buffer inicializado todo a 0's
    unsigned char buffer0[BLOCKSIZE];
    memset(buffer0, 0, BLOCKSIZE);

    //Montamos el dispositivo virtual
    if(bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED "Error al montar el dispositivo\n" RESET);
        return FALLO;
    }

    //Inicializamos a 0's el fichero utilizado como dispositivo virtual
    for (int i=0; i<atoi(argv[2]); i++) {        
        if (bwrite(i, buffer0) == FALLO) { //Si hay error al escribir desmontamos el dispositivo
           fprintf(stderr, RED "Error inicializar el dispositivo a 0's\n" RESET);
           bumount();
           return FALLO;
        }
    }

    //Desmontamos el dispositivo virtual
    if (bumount() == FALLO) {
        return FALLO;
    }
}