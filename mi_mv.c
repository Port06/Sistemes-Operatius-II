#include "directorios.h"
#include "bloques.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr,
                "Uso: ./mi_mv <disco> </origen> </destino>\n");
        fprintf(stderr,
                "  Si destino termina en '/', se mueve dentro manteniendo nombre\n");
        fprintf(stderr,
                "  Si destino no termina en '/', se mueve renombrando\n");
        return FALLO;
    }

    // Montar disco
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar el disco\n");
        return FALLO;
    }

    // Detectar si origen es fichero o directorio
    int origen_es_directorio = 0;
    int len_origen = strlen(argv[2]);
    if (argv[2][len_origen - 1] == '/') {
        origen_es_directorio = 1;
    }

    // Detectar si destino es directorio
    int destino_es_directorio = 0;
    int len_destino = strlen(argv[3]);
    if (argv[3][len_destino - 1] == '/') {
        destino_es_directorio = 1;
    }

    // Construir destino final
    char destino_final[1024];
    
    if (destino_es_directorio) {
        // Mover dentro del directorio manteniendo nombre
        // Obtener nombre del origen
        const char *nombre_origen = strrchr(argv[2], '/');
        if (nombre_origen == NULL) {
            fprintf(stderr, "Error: ruta de origen inválida\n");
            bumount();
            return FALLO;
        }
        nombre_origen++; // Saltar la barra
        
        // Si origen es directorio, quitar barra final del nombre
        char nombre_limpio[256];
        strcpy(nombre_limpio, nombre_origen);
        if (origen_es_directorio && nombre_limpio[strlen(nombre_limpio)-1] == '/') {
            nombre_limpio[strlen(nombre_limpio)-1] = '\0';
        }
        
        // Construir nueva ruta
        sprintf(destino_final, "%s%s", argv[3], nombre_limpio);
        if (origen_es_directorio) {
            strcat(destino_final, "/");
        }
    } else {
        // Mover renombrando
        strcpy(destino_final, argv[3]);
    }

    // Verificar que no se intenta mover un directorio a un fichero
    if (origen_es_directorio && !destino_es_directorio) {
        fprintf(stderr, "Error: no se puede mover un directorio a un fichero\n");
        bumount();
        return FALLO;
    }

    // Verificar que no se mueve un directorio dentro de sí mismo
    if (origen_es_directorio) {
        if (strncmp(argv[2], destino_final, strlen(argv[2])) == 0) {
            fprintf(stderr, "Error: no se puede mover un directorio dentro de sí mismo\n");
            bumount();
            return FALLO;
        }
    }

    // Realizar movimiento: link + unlink
    int error = mi_link(argv[2], destino_final);
    if (error < 0) {
        fprintf(stderr, "Error al crear enlace en destino\n");
        bumount();
        return FALLO;
    }

    error = mi_unlink(argv[2]);
    if (error < 0) {
        fprintf(stderr, "Error al eliminar origen, deshaciendo cambios...\n");
        mi_unlink(destino_final);
        bumount();
        return FALLO;
    }

    printf("Movimiento exitoso: %s -> %s\n", argv[2], destino_final);

    // Desmontar disco
    bumount();

    return EXITO;
}