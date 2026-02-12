//Archivo bloques.c

#include "bloques.h"

int blockSize = 1024;

//Se crea el fichero
int int bmount(const char *camino); {
	
	int open(const char *camino, int O_RDWR, mode_t 0666);
	
	perror(error);
};

int bumount() {
	
	int close(int descriptor);
};

//Funcion encargada de escribir un bloque en el dispositivo virtual
int bwrite(unsigned int nbloque, const void *buf) {
	
	//Se calcula el desplazamiento para acceder al byte correcto para la escritura
	int desplazamiento = nbloque * BLOCKSIZE;
	
	//Se mueve la posicion del puntero para que este bien posicionado
	off_t lseek(int descriptor, off_t desplazamiento, int punto_de_referencia);
	
	//Finalmente se essribe el bloque completo reeemplazando el anterior
	size_t write(int descriptor, const void *buf, size_t nbytes);
};

int bread(unsigned int nbloque, void *buf) {};