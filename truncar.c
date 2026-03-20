#include <ficheros.h>

int main(int argc, char **argv){

    //Sintaxis de la ejecucion truncar <nombre_dispositivo> <ninodo> <nbytes>
    if(argc != 4){
        fprintf(stderr,"Sintaxis: ./truncar <disco> <ninodo> <nbytes>\n");
        return FALLO;
    }

    bmount(argv[1]);

    if(atoi(argv[3]) == 0){
        liberar_inodo(atoi(argv[2]));
    }else{
        mi_truncar_f(atoi(argv[2]), atoi(argv[3]));
    }

    // Obtener estado antes
struct STAT stat;
mi_stat_f(atoi(argv[2]), &stat);
// Mostrar valores antes...

// Hacer truncar/liberar

// Obtener estado después
mi_stat_f(atoi(argv[2]), &stat);
// Mostrar valores después para verificar

printf("tamEnBytesLog: %d\n", stat.tamEnBytesLog);
printf("numBloquesOcupados: %d\n", stat.numBloquesOcupados);
}