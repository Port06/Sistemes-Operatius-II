//Archivo fichero_basico.c

#include "bloques.h"
#include "ficheros_basico.h"
#include <time.h>
#include <limits.h>


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

//Metodo que inizializa a los bits del los metadatos
int initMB() {
	struct superbloque SB;
	
	unsigned int bloquesMetadatos;
	bloquesMetadatos = SB.posPrimerBloqueDatos;

	unsigned int bitsMetadatos;
	unsigned int bytesCompletos;
	unsigned int bitsResto;

	bitsMetadatos = bloquesMetadatos;     //Cada bloque es 1 bit

	bytesCompletos = bitsMetadatos / 8;   //Los bytes completos a 11111111
	bitsResto      = bitsMetadatos % 8;   //Los bits sueltos del siguiente byte
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
	SB.totInodos = ninodos;

	//Se comprueba que se haya escrito bien el superbloque
	if (bwrite(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al escribir la estructura en SB\n" RESET);
		return FALLO;
	}

}

//Metodo para inizializar los inodos libres
int initAI(){
	struct superbloque SB;
	if (bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	struct inodo inodos [BLOCKSIZE/INODOSIZE];
	int ContInodos = SB.posPrimerInodoLibre+1;
	for(int i=SB.posPrimerBloqueAI; i<=SB.posUltimoBloqueAI; i++){
		if(bread(i, &inodos) == FALLO) {
			fprintf(stderr, RED "Error al leer la estructura en AI\n" RESET);
			return FALLO;
		}
		for(int j=0; j<BLOCKSIZE/INODOSIZE; j++){
			inodos[j].tipo = 'l';
			if(ContInodos<SB.totInodos){
				inodos[j].punterosDirectos[0] = ContInodos;
				ContInodos++;
			}else{
				inodos[j].punterosDirectos[0] = UINT_MAX;
				break;
			}
		
		}
		if(bwrite(i, &inodos) == FALLO) {
			fprintf(stderr, RED "Error al escribir la estructura en AI\n" RESET);
			return FALLO;
		}
	
	
}
return EXITO;
}