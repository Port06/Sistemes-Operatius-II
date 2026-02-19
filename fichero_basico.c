//Archivo fichero_basico.c

#include "bloques.h"
#include "fichero_basico.h"
#include <time.h>


//Metodo que inizializa el mapa de bits
int initMB() {
	unsigned int bloquesMetadatos;
	bloquesMetadatos = tamSB + tamMB + tamAI;
	
	unsigned int bitsMetadatos;
	unsigned int bytesCompletos;
	unsigned int bitsResto;

	bitsMetadatos = bloquesMetadatos;     //Cada bloque es igual a 1 bit

	bytesCompletos = bitsMetadatos / 8;   //Los bytes completos a 11111111
	bitsResto = bitsMetadatos % 8;   //Los bits sueltos del siguiente byte
		
};
int initAI();
int initSB(unsigned int nbloques, unsigned int ninodos);

//Metodo para clacular el numero necessario de bloques
int tamMB(unsigned int nbloques) {
	int bloques = 0;
	
	//Se calcula el numero de bloques en fraccion
	bloques = (nbloques / 8) / BLOCKSIZE;
	
	//Luego se incrementa el numero de bloques en 1 si no es division exacta
	if (((nbloques / 8) % BLOCKSIZE) != 0) bloques++;
	
	return bloques;
};

//Metodo para calcular el tamano en boloques de array de inodos
int tamAI(unsigned int ninodos) {
	int tamAI = 0;
	
	//Se calcula el numero de bloques para el array inodos en fraccion
	tamAI = (ninodos * INODOSIZE) / BLOCKSIZE; 
	
	//Luego de incrementa el numero de bolques en 1 si la division no es exacta
	if (((ninodos / 8) % BLOCKSIZE) != 0) tamAI++;
	
};

//Metodo que iniziliza el superbloque
int initSB(unsigned int nbloques, unsigned int ninodos) {
	struct superbloque SB;

	SB.posPrimerBloqueMB = posSB + tamSB; //posSB = 0, tamSB = 1
	SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
	SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
	SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
	SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
	SB.posUltimoBloqueDatos = nbloques-1;
	SB.posInodoRaiz = 0;
	SB.posPrimerInodoLibre = 0;
	SB.cantBloquesLibres = nbloques;
	SB.cantInodosLibres = ninodos;
	SB.totBloques = nbloques;
	SB.totinodos = ninodos

	//Se comprueba que se haya escrito bien el superbloque
	if (bwrite(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al escribir la estructura en SB\n" RESET);
		return FALLO;
	}

}