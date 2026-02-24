#include "ficheros_basico.h"

int main(int argc, char **argv){
    struct superbloque SB;

    char *nombreArchivo = argv[1];

    if(argc != 2) {
        perror("Error, faltan o sobran argumentos: $ ./leer_sf <nombre_dispositivo>");
        return FALLO;
    } 

    if(bmount(nombreArchivo) == FALLO) return FALLO;

    if(bread(posSB, &SB) == FALLO) {
        perror(RED "Error al leer el superbloque");
        return FALLO;
    }


    printf("DATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %d\n", SB.posUltimoBloqueMB);
    printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);
    printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    printf("posInodoRaiz = %d\n", SB.posInodoRaiz);
    printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    printf("cantBloquesLibres = %d\n",SB.cantBloquesLibres);
    printf("cantInodosLibres = %d\n",SB.cantInodosLibres);
    printf("totBloques = %d\n", SB.totBloques);
    printf("totInodos = %d\n\n", SB.totInodos);
}