#include "ficheros_basico.h"
#include <time.h>

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

	struct tm *ts;
	char atime[80];
	char mtime[80];
	char ctime[80];
	char btime[80];


<<<<<<< HEAD
struct inodo inodo;
int ninodo = 0;
=======
	struct inodo inodo;
	int ninodo;
>>>>>>> c1202a8adeaf2934af39b23b9d1e40d133b78c62

	leer_inodo(ninodo, &inodo);
	ts = localtime(&inodo.atime);
	strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&inodo.mtime);
	strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&inodo.ctime);
	strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&inodo.btime);
	strftime(ctime, sizeof(btime), "%a %Y-%m-%d %H:%M:%S", ts);
	printf("ID: %d ATIME: %s MTIME: %s CTIME: %s BTIME: %s\\n",ninodo,atime,mtime,ctime, btime);
}