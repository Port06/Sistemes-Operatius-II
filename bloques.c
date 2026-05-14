//Archivo bloques.c

#include "bloques.h"
#include "semaforo_mutex_posix.h"

// Inicializacion de variables
static int descriptor = 0;
static sem_t *mutex;
static unsigned int inside_sc = 0;

// Metodos para los semaforos
void mi_waitSem() {

    if (!inside_sc) {
        waitSem(mutex);
    }

    inside_sc++;
}

void mi_signalSem() {

    inside_sc--;

    if (!inside_sc) {
        signalSem(mutex);
    }
}


// Se crea el fichero
int bmount(const char *camino) {

    // Si ya hay un descriptor abierto lo cerramos
    // (importante después de fork())
    if (descriptor > 0) {

        if (close(descriptor) == -1) {
            fprintf(stderr, RED "Error al cerrar descriptor previo\n" RESET);
            return FALLO;
        }
    }

    // Inicializar semáforo solo una vez
    if (!mutex) {

        mutex = initSem();

        if (mutex == SEM_FAILED) {
            fprintf(stderr, RED "Error al inicializar el semáforo\n" RESET);
            return FALLO;
        }
    }

    // Abrir dispositivo virtual
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);

    if (descriptor == -1) {
        fprintf(stderr, RED "Error al abrir el descriptor\n" RESET);
        return FALLO;
    }

    return descriptor;
}

//Se cierra el fichero
int bumount() {

    descriptor = close(descriptor);

    if (descriptor == -1) {
        fprintf(stderr, RED "Error al cerrar el dispositivo\n" RESET);
        return FALLO;
    }

    if (mutex) {
        deleteSem(mutex);
        mutex = NULL;
        inside_sc = 0;
    }

    return EXITO;
}

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
	if (bytes_escritos == -1) {
		fprintf(stderr, RED "Error al escribir\n" RESET);
		return FALLO;
	}

	if (bytes_escritos != BLOCKSIZE) {
		fprintf(stderr, RED "Escritura incompleta\n" RESET);
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
	if (bytes_leidos == -1) {
        fprintf(stderr, RED "Error al verificar\n" RESET);
        return FALLO;
    }
	
	if (bytes_leidos != BLOCKSIZE) {
		fprintf(stderr, RED "Lectura incompleta\n" RESET);
		return FALLO;
	}
	
	return bytes_leidos;
};