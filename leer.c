#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // write()
#include "ficheros.h"
#include "ficheros_basico.h"

int main(int argc, char **argv){

    // Validación de argumentos
    if (argc != 3){
        fprintf(stderr,"Sintaxis: ./leer <nombre_dispositivo> <ninodo>\n");
        return -1;
    }

    // Montar dispositivo
    if (bmount(argv[1]) == -1){
        fprintf(stderr,"Error al montar el dispositivo\n");
        return -1;
    }

    // Obtener número de inodo
    unsigned int ninodo = atoi(argv[2]);

    // Obtener información del inodo
    struct STAT stat;
    if (mi_stat_f(ninodo, &stat) == -1){
        fprintf(stderr,"Error al obtener información del inodo\n");
        bumount();
        return -1;
    }

    // Tamaño del buffer de lectura (modificable fácilmente)
    const int tambuffer = BLOCKSIZE;
    
    // Reservar buffer de lectura
    char buffer_texto[tambuffer];
    
    unsigned int offset = 0;
    int leidos;
    int total_leidos = 0;

    // Limpiar buffer antes de la primera lectura
    memset(buffer_texto, 0, tambuffer);
    leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);

    // Bucle de lectura hasta EOF
    while (leidos > 0){
        // Mostrar contenido leído por salida estándar
        write(1, buffer_texto, leidos);
        
        // Acumular bytes leídos
        total_leidos += leidos;
        
        // Avanzar offset
        offset += tambuffer;
        
        // Limpiar buffer antes de la siguiente lectura
        memset(buffer_texto, 0, tambuffer);
        leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);
    }

    // Mostrar estadísticas por stderr (no se mezcla con redirección >)
    char string[256];
    sprintf(string, "\nTotal bytes leídos: %d\n", total_leidos);
    write(2, string, strlen(string));
    
    sprintf(string, "tamEnBytesLog: %d\n", stat.tamEnBytesLog);
    write(2, string, strlen(string));

    // Desmontar dispositivo
    bumount();

    return 0;
}