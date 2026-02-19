//Archivo bloques.c

#include "bloques.h"

//Inicializacion de variables
static int descriptor = 0;


//Se crea el fichero
int bmount(const char *camino) {
	
	descriptor = open(camino, O_RDWR | O_CREAT, 0666);
	
	//Manejo de errores al abrir el descriptor
	if (descriptor == -1) {
		fprintf(stderr, RED "Error al abrir el descriptor\n" RESET);
		return FALLO;
	}
	return descriptor;
};

//Se cierra el fichero
int bumount() {
	
	//Manejo e errores al cerrar el descriptor
	if (close(descriptor) == -1) {
		fprintf(stderr, RED "Error al cerrar el dispositivo\n" RESET);
		return FALLO;
	}
	return EXITO;
};

//Funcion encargada de escribir un bloque en el dispositivo virtual
int bwrite(unsigned int nbloque, const void *buf) {
	
	//Se calcula el desplazamiento para acceder al byte correcto para la escritura
	off_t desplazamiento = nbloque * BLOCKSIZE;
	
	//Se mueve la posicion del puntero para que este bien posicionado
	if (lseek(descriptor, desplazamiento, SEEK_SET) == -1) {
        fprintf(stderr, RED "Error al escribir\n" RESET);
        return FALLO;
    }
	
	//Finalmente se essribe el bloque completo reeemplazando el anterior
	int bytes_escritos = write(descriptor, buf, BLOCKSIZE);
	
	//Verificamos que se hayan escrito los bytes correctos
	if (bytes_escritos != BLOCKSIZE) {
		fprintf(stderr, RED "Error al escribir\n" RESET);
		return FALLO;
	} else if (bytes_escritos == -1) {
        fprintf(stderr, RED "Error al verificar\n" RESET);
        return FALLO;
    }
	
	return bytes_escritos;
};

int bread(unsigned int nbloque, void *buf) {
	//Se calcula el desplazamiento para acceder al byte correcto para la lectura
	off_t desplazamiento = nbloque * BLOCKSIZE;
	
	//Se mueve la posicion del puntero para que este bien posicionado
	if (lseek(descriptor, desplazamiento, SEEK_SET) == -1) {
        fprintf(stderr, RED "Error al leer\n" RESET);
        return FALLO;
    }
	
	//Se efectua la lectura del bloque completo y se almacena en el buffer
	int bytes_leidos = read(descriptor, buf, BLOCKSIZE);
	
	//Verificamos que se hayan leido los bytes correctos
	if (bytes_leidos != BLOCKSIZE) {
		fprintf(stderr, RED "Lectura incompleta\n" RESET);
		return FALLO;
	} else if (bytes_leidos == -1) {
        fprintf(stderr, RED "Error al verificar\n" RESET);
        return FALLO;
    }
	
	return bytes_leidos;
};