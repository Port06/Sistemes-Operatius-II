#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // write()
#include "ficheros.h"
#include "ficheros_basico.h"

int main(int argc, char **argv){

    // Validación de argumentos
    if (argc != 4){
        fprintf(stderr,"Sintaxis: ./escribir <disco> <texto> <diferentes_inodos>\n");
        return -1;
    }

    // Se monta el dispositivo
    if (bmount(argv[1]) == -1){
        fprintf(stderr,"Error al montar el dispositivo\n");
        return -1;
    }

    // Texto a escribir
    char *texto = argv[2];

    // Flag: 0 → mismo inodo, 1 → uno distinto por offset
    int diferentes_inodos = atoi(argv[3]);

    // Offsets de prueba (distintos tipos de punteros)
    unsigned int offsets[5] = {
        9000,        // directo
        209000,      // indirecto simple
        30725000,    // indirecto doble
        409605000,   // indirecto triple
        480000000    // triple profundo
    };

    unsigned int ninodo;

    // Se reserva el primer inodo
    ninodo = reservar_inodo('f', 6);
    printf("Nº inodo reservado: %d\n", ninodo);

    // Bucle de escritura
    for(int i = 0; i < 5; i++){

        // Si necesario, reservar un nuevo inodo en cada iteración
        if(diferentes_inodos == 1){
            ninodo = reservar_inodo('f', 6);
            printf("Nº inodo reservado: %d\n", ninodo);
        }

        printf("\nOffset: %d\n", offsets[i]);

        // Escritura
        int escritos = mi_write_f(
            ninodo,
            texto,
            offsets[i],
            strlen(texto)
        );

        printf("Bytes escritos: %d\n", escritos);

        // Se obtiene información del inodo
        struct STAT stat;

        mi_stat_f(ninodo, &stat);

        printf("tamEnBytesLog: %d\n", stat.tamEnBytesLog);
        printf("numBloquesOcupados: %d\n", stat.numBloquesOcupados);
    }

    // Desmontar dispositivo
    bumount();

    return 0;
}