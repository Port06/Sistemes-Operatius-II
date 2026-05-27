#include "directorios.h"
#include "bloques.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Sintaxis: %s <nombre_dispositivo> </ruta_fichero>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *nombre_dispositivo = argv[1];
    char *camino = argv[2];

    if (bmount(nombre_dispositivo) == -1) {
        perror("Error al montar dispositivo");
        exit(EXIT_FAILURE);
    }

    struct STAT stat;
    if (mi_stat(camino, &stat) < 0) {
        fprintf(stderr, "Error al obtener stat del fichero\n");
        bumount();
        exit(EXIT_FAILURE);
    }

    if (stat.tipo != 'f') {
        fprintf(stderr, "Error: la ruta no corresponde a un fichero\n");
        bumount();
        exit(EXIT_FAILURE);
    }

    unsigned char buffer[TAMBUFFER];
    int leidos;
    unsigned int offset = 0;
    unsigned int total_leidos = 0;

    memset(buffer, 0, TAMBUFFER);
    while ((leidos = mi_read(camino, buffer, offset, TAMBUFFER)) > 0) {
		write(1, buffer, leidos);
		offset += leidos;
		total_leidos += leidos;
		memset(buffer, 0, TAMBUFFER);
	}

	if (leidos < 0) {
		fprintf(stderr, "Error al leer el fichero\n");
		bumount();
		exit(EXIT_FAILURE);
	}

    fprintf(stderr, "\nTotal leídos: %u bytes\n", total_leidos);

    if (bumount() == -1) {
        perror("Error al desmontar dispositivo");
        exit(EXIT_FAILURE);
    }

    return 0;
}