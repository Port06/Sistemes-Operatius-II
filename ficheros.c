#include “bloques.h”
#include “ficheros_basico.h”
#include <time.h>
#include <limits.h>


int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
	int buf_bloque;
	int desp1; // Desplazamiento en el bloque parte de la izquierda
	int desp2; // Desplazamiento en el bloque parte de la derecha
	
	// Primer bloque logico (preservar desp1)
	buf_bloque = traducir_bloque_inodo (ninodo, primerBLogico, 1)
	
	desp1 = offset % BLOCKSIZE;
	desp2 = BLOCKSIZE - desp1;
	
	memcpy (buf_bloque + 808, buf_original, 216);	
	bwrite(desp1, buf_original);
	
	// Bloques logicos intermedios
	buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE;
	bwrite(nbfisico, buf_original + (1024 – 808) + (bl – 8 – 1) * 1024);
	
	// Ultimo bloque logico (preservar desp2)
	buf_bloque = traducir_bloque_inodo(ninodo, ultimoBLogico, 1);
	memcpy (buf_bloque, buf_original + (3751 – 462 - 1), 462 + 1);
	bwrite(desp2, buf_bloque);
};

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {

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