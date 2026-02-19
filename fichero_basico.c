//Archivo fichero_basico.c

#include "bloques.h"
#include "fichero_basico.h"
#include <time.h>

int initMB();
int initAI();
int initSB(unsigned int nbloques, unsigned int ninodos);

//Metodo para clacular el numero necessario de bloques
int tamMB(unsigned int nbloques) {
	int bloques = 0;
	
	//Se calcula el numero de bloques con fraccion
	bloques = (nbloques / 8) / BLOCKSIZE;
	
	//Luego se incremente el numero de bloques en 1 si no es division eacta
	if (((nbloques / 8) % BLOCKSIZE) != 0) bloques++;
	
	return bloques;
};
int tamAI(unsigned int ninodos);