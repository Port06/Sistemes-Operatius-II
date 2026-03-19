#include “bloques.h”
#include “ficheros_basico.h”
#include <time.h>
#include <limits.h>


int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
	struct inodo inodo;
    unsigned int primerBL, ultimoBL; // Bloque inicial y final de la escritura
    unsigned int desp1, desp2; // Desplazamientos dentro de los bloques necesarios para el primer y ultimo bloque
    unsigned int nbfisico; // BLoque de memoria existente en la memoria
    unsigned int bytesEscritos = 0;
    unsigned char buf_bloque[BLOCKSIZE];
	
	mi_read_f(ninodo, &inodo); // O leer_inodo

	if ((inodo.permisos & 2) != 2) {
    fprintf(stderr, "No hay permisos de escritura\n");
    return FALLO;
	}
	
	// Calcular los desplazamientos y bloques de escritura
	primerBL = offset / BLOCKSIZE;
	ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

	desp1 = offset % BLOCKSIZE;
	desp2 = (offset + nbytes - 1) % BLOCKSIZE;
	
	//Caso 1: primerBL == ultimoBL
	if (primerBL == ultimoBL){

		nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1); // Almacenar el contenido existente de la memoria
		bread(nbfisico, buf_bloque);

		memcpy(buf_bloque + desp1, buf_original, nbytes); // Combinacion del bloque existente y del contenido a escribir

		bwrite(nbfisico, buf_bloque); // Escritura del nuevo contenido del bloque a memoria
		bytesEscritos = nbytes;
	}
	else{ //Caso 2: primerBL != ultimoBL

		// Primer bloque
		nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1); // Almacenar el contenido existente de la memoria
		bread(nbfisico, buf_bloque);

		memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);

		bwrite(nbfisico, buf_bloque); // Escritura del nuevo contenido del bloque a memoria
		bytesEscritos += BLOCKSIZE - desp1;
		
		// Bloques intermediso
		for(int bl = primerBL + 1; bl < ultimoBL; bl++){ // Para cada bloque de memoria que no sea ni el primero ni el ultimo

			nbfisico = traducir_bloque_inodo(ninodo, bl, 1); // Obtener el contenido de la memoria para cada bloque

			// Escritura del nuevo contenido del bloque a memoria
			bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE);
			bytesEscritos += BLOCKSIZE;
		}
		
		// Ultimo bloque	
		nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1); // Almacenar el contenido existente de la memoria
		bread(nbfisico, buf_bloque);

		memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1); // Combinacion del bloque existente y del contenido a escribir

		bwrite(nbfisico, buf_bloque); // Escritura del nuevo contenido del bloque a memoria
		bytesEscritos += desp2 + 1;
	}	
	
	// Lectura del inodo para actualizar sus metadatos
	leer_inodo(ninodo, &inodo);

	// Si la escritura ha sobrepasado el tamano logico actual del fichero,
	// significa que hemos ampliado el fichero y debemos actualizar tamEnBytesLog
	// El nuevo tamaño será la ultima posicion escrita: offset + nbytes
	if (offset + nbytes > inodo.tamEnBytesLog){
		inodo.tamEnBytesLog = offset + nbytes;
	}

	inodo.mtime = time(NULL);
	inodo.ctime = time(NULL);

	// Guardamos el inodo actualizado en disco
	escribir_inodo(ninodo, inodo);
	
	return bytesEscritos;
};

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {

	struct inodo inodo;
	unsigned int bytesLeidos = 0;
	if(leer_inodo(ninodo, &inodo) == FALLO) {
		fprintf(stderr, RED "Error al leer el inodo en mi_read_f\n" RESET);
		return FALLO;
	}

	if((inodo.permisos & 4) != 4) {
		fprintf(stderr, RED "Error, el inodo no tiene permisos de lectura\n" RESET);
		return FALLO;
	}

	if(offset > inodo.tamEnBytesLog) {
		fprintf(stderr, RED "Error, el offset es mayor que el tamaño lógico del fichero\n" RESET);
		bytesLeidos = 0;
		return bytesLeidos;
	}else{
		if((offset + nbytes) >= inodo.tamEnBytesLog) {
			nbytes = inodo.tamEnBytesLog - offset;
		}
	}

	int primerBL = offset / BLOCKSIZE;
	int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
	desp1 = offset % BLOCKSIZE;

	
	

};

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat) {
	struct STAT inodo;
	if(leer_inodo(ninodo, &inodo) == FALLO) {
		fprintf(stderr, RED "Error al leer el inodo en mi_stat_f\n" RESET);
		return FALLO;
	}

	p_stat->tipo = inodo.tipo;
	p_stat->permisos = inodo.permisos;
	p_stat->atime = inodo.atime;
	p_stat->mtime = inodo.mtime;
	p_stat->ctime = inodo.ctime;
	p_stat->nlinks = inodo.nlinks;
	p_stat->tamEnBytes = inodo.tamEnBytesLog;
	p_stat->numBloquesOcupados = inodo.numBloquesOcupados;

	return EXITO;

};

int mi_chmod_f(unsigned int ninodo, unsigned char permisos) {
	struct inodo inodo;

	if(leer_inodo(ninodo, &inodo) == FALLO) {
		fprintf(stderr, RED "Error al leer el inodo en mi_chmod_f\n" RESET);
		return FALLO;
	}


	inodo.permisos = permisos;
	inodo.ctime = time(NULL);

	if(escribir_inodo(ninodo, &inodo) == FALLO) {
		fprintf(stderr, RED "Error al escribir el inodo en mi_chmod_f\n" RESET);
		return FALLO;
	}
	return EXITO;

};

 int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);