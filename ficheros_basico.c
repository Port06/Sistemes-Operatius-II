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
	
	return tamAI;
};

//Metodo que inizializa a los bits del los metadatos
int initMB() {
	struct superbloque SB;
	//Leemos el superbloque
	if (bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	
	unsigned int bloquesMetadatos;
	unsigned int bytesCompletos;
	unsigned int bitsResto;
	
	bloquesMetadatos = SB.posPrimerBloqueDatos;

	unsigned int bitsMetadatos;

	bitsMetadatos = bloquesMetadatos;     //Cada bloque es 1 bit

	bytesCompletos = bitsMetadatos / 8;   //Los bytes completos a 11111111
	bitsResto = bitsMetadatos % 8;   //Los bits sueltos del siguiente byte

	return EXITO;
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

	return EXITO;
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

int escribir_bit(unsigned int nbloque, unsigned int bit){

	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	int posbyteMB = nbloque / 8;
	int posbit = nbloque % 8;

	int nbloqueMB = posbyteMB / BLOCKSIZE;

	int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

	unsigned char bufferMB[BLOCKSIZE];

	if (bread(SB.posPrimerBloqueMB + nbloqueMB, bufferMB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}

	int posbyte = posbyteMB % BLOCKSIZE;

	unsiged char mascara = 128; //10000000
	mascara >>= posbit; //Desplazamos la mascara a la derecha segun el numero de bit

	if(bit == 1){
		bufferMB[posbyte] |= mascara; //Pone a 1 el bit
	}else{
		bufferMB[posbyte] &= ~mascara; //Pone a 0 el bit
	}

	if (bwrite(SB.posPrimerBloqueMB + nbloqueMB, bufferMB) == -1) {
		return FALLO;
	}

    return EXITO;
	
	
}

char leer_bit(unsiged int nbloque){
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}

	//Mismo tratamiento que en escribir_bit para calcular la posicion del byte y el bit dentro del bloque de metadatos
	int posbyteMB = nbloque / 8;
	int posbit = nbloque % 8;
	int nbloqueMB = posbyteMB / BLOCKSIZE;
	int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

	//Volvemos a necesitar un buffer
	unsigned char bufferMB[BLOCKSIZE];

	//Tratamiento real del metodo de leer_bit,
	if(bread(nbloqueabs, bufferMB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}

	unsigned char mascara = 128; // 10000000
	mascara >>= posbit;          // desplazamiento de bits a la derecha, los que indique posbit
	mascara &= bufferMB[posbyte]; // operador AND para bits
	mascara >>= (7 - posbit);     // desplazamiento de bits a la derecha 
                                // para dejar el 0 o 1 en el extremo derecho y leerlo en decimal
	
	return mascara;
}

int reservar_bloque() {
	
}

int liberar_bloque(unsigned int nbloque) {
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}

	if(escribir_bit(nbloque, 0) == FALLO) {
		fprintf(stderr, RED "Error al escribir el bit en liberar_bloque\n" RESET);
		return FALLO;
	}
	
	SB.cantBloquesLibres++;

	if (bwrite(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al escribir la estructura en SB\n" RESET);
		return FALLO;
	}

	return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo) {
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}

	int nbloqueAI = (ninodo * INODOSIZE) / BLOCKSIZE;
	int nbloqueabs = SB.posPrimerBloqueAI + nbloqueAI;

	struct inodo inodos[BLOCKSIZE/INODOSIZE];

	if(bread(nbloqueabs, inodos) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en AI\n" RESET);
		return FALLO;
	}

	int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);
	inodos[posinodo] = *inodo;

	if(bwrite(nbloqueabs, inodos) == FALLO) {
		fprintf(stderr, RED "Error al escribir la estructura en AI\n" RESET);
		return FALLO;
	}

	return EXITO;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo) {
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}

	//Calculamos el bloque del array de inodos donde se encuentra el inodo que queremos leer (igual que en escribir_inodo)
	int nbloqueAI = (ninodo * INODOSIZE) / BLOCKSIZE;
	int nbloqueabs = SB.posPrimerBloqueAI + nbloqueAI;

	struct inodo inodos[BLOCKSIZE/INODOSIZE];
	if(bread(nbloqueabs, inodos) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en AI\n" RESET);
		return FALLO;
	}

	//Calculamos la posicion del inodo dentro del bloque de inodos
	int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);
	*inodo = inodos[posinodo];

	return EXITO;
	
	
}

int reservar_inodo(unsigned char tipo, unsigned char permisos) {
	
}