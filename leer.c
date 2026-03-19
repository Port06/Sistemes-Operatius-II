#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ficheros.h"

int main(int argc, char **argv){

    if (argc != 4){
        fprintf(stderr,"Sintaxis: ./escribir <disco> <texto> <diferentes_inodos>\n");
        return -1;
    }

    // Montar dispositivo
    if (bmount(argv[1]) == -1){
        fprintf(stderr,"Error al montar\n");
        return -1;
    }

    char *texto = argv[2];
    int diferentes_inodos = atoi(argv[3]);

	// Cada offset está diseñado para probar un tipo distinto
    // de puntero dentro del inodo (directo, indirecto simple, doble o triple).
    unsigned int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};

    unsigned int ninodo;

    // Reservar primer inodo
    ninodo = reservar_inodo('f',6);

    printf("Nº inodo reservado: %d\n", ninodo);

    for(int i=0; i<5; i++){

		// Si necesario, reservar un nuevo inodo en cada iteración
        if(diferentes_inodos == 1){
            ninodo = reservar_inodo('f',6);
            printf("Nº inodo reservado: %d\n", ninodo);
        }

        printf("\nOffset: %d\n", offsets[i]);

		// Lectura
        int escritos = mi_write_f(ninodo, texto, offsets[i], strlen(texto));

        printf("Bytes escritos: %d\n", escritos);

		// Se obtiene información del inodo
        struct STAT stat;

        mi_stat_f(ninodo,&stat);

        printf("tamEnBytesLog: %d\n", stat.tamEnBytesLog);
        printf("numBloquesOcupados: %d\n", stat.numBloquesOcupados);
    }

	// Desmontar dispositivo
    bumount();

    return 0;
}