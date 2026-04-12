#include "ficheros.h"
#include "ficheros_basico.h"
#include <time.h>
#include <limits.h>


int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    unsigned int primerBL, ultimoBL; // Bloque inicial y final de la escritura
    unsigned int desp1, desp2; // Desplazamientos dentro de los bloques necesarios para el primer y ultimo bloque
    unsigned int nbfisico; // Bloque físico existente en disco
    unsigned int bytesEscritos = 0;
    unsigned char buf_bloque[BLOCKSIZE];
    const char *buf = (const char *)buf_original;
    
    // O leer_inodo
    if (leer_inodo(ninodo, &inodo) == FALLO) {
        return FALLO;
    }

    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, "No hay permisos de escritura\n");
        return FALLO;
    }

    if (nbytes == 0) {
        return 0;
    }
    
    // Calcular los desplazamientos y bloques de escritura
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;
    
    //Caso 1: primerBL == ultimoBL
    if (primerBL == ultimoBL) {

        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        if (nbfisico == FALLO) return FALLO;

        if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;

        memcpy(buf_bloque + desp1, buf, nbytes); // Combinacion del bloque existente y del contenido a escribir

        if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO; // Escritura del nuevo contenido del bloque a memoria
        bytesEscritos = nbytes;
    }
    else { //Caso 2: primerBL != ultimoBL

        // Primer bloque
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        if (nbfisico == FALLO) return FALLO;

        if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;

        memcpy(buf_bloque + desp1, buf, BLOCKSIZE - desp1);

        if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO; // Escritura del nuevo contenido del bloque a memoria
        bytesEscritos += BLOCKSIZE - desp1;
        
        // Bloques intermedios
        unsigned int offset_buf = BLOCKSIZE - desp1;
        for (unsigned int bl = primerBL + 1; bl < ultimoBL; bl++) { // Para cada bloque de memoria que no sea ni el primero ni el ultimo

            nbfisico = traducir_bloque_inodo(ninodo, bl, 1);
            if (nbfisico == FALLO) return FALLO;

            // Escritura del nuevo contenido del bloque a memoria
            if (bwrite(nbfisico, (void *)(buf + offset_buf)) == FALLO) return FALLO;
            bytesEscritos += BLOCKSIZE;
            offset_buf += BLOCKSIZE;
        }
        
        // Ultimo bloque
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
        if (nbfisico == FALLO) return FALLO;

        if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;

        memcpy(buf_bloque, buf + (nbytes - (desp2 + 1)), desp2 + 1); // Combinacion del bloque existente y del contenido a escribir

        if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO; // Escritura del nuevo contenido del bloque a memoria
        bytesEscritos += desp2 + 1;
    }    
    
    // Lectura del inodo para actualizar sus metadatos
    if (leer_inodo(ninodo, &inodo) == FALLO) return FALLO;

    // Si la escritura ha sobrepasado el tamano logico actual del fichero,
    // significa que hemos ampliado el fichero y debemos actualizar tamEnBytesLog
    // El nuevo tamaño será la ultima posicion escrita: offset + nbytes
    if (offset + nbytes > inodo.tamEnBytesLog) {
        inodo.tamEnBytesLog = offset + nbytes;
    }

    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);

    // Guardamos el inodo actualizado en disco
    if (escribir_inodo(ninodo, &inodo) == FALLO) return FALLO;
    
    return bytesEscritos;
};

// int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {

// 	struct inodo inodo;
// 	unsigned int bytesLeidos = 0;
// 	if(leer_inodo(ninodo, &inodo) == FALLO) {
// 		fprintf(stderr, RED "Error al leer el inodo en mi_read_f\n" RESET);
// 		return FALLO;
// 	}

// 	if((inodo.permisos & 4) != 4) {
// 		fprintf(stderr, RED "Error, el inodo no tiene permisos de lectura\n" RESET);
// 		return FALLO;
// 	}

// 	if(offset > inodo.tamEnBytesLog) {
// 		fprintf(stderr, RED "Error, el offset es mayor que el tamaño lógico del fichero\n" RESET);
// 		bytesLeidos = 0;
// 		return bytesLeidos;
// 	}else{
// 		if((offset + nbytes) >= inodo.tamEnBytesLog) {
// 			nbytes = inodo.tamEnBytesLog - offset;
// 		}
// 	}

// 	int primerBL = offset / BLOCKSIZE;
// 	int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
// 	int desp1 = offset % BLOCKSIZE;

// };

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    struct inodo inodo;
    unsigned int primerBL, ultimoBL, nbfisico;
    int desp1, desp2, nbytesLeidos = 0;
    char buf_bloque[BLOCKSIZE];

    // Leer inodo
    if (leer_inodo(ninodo, &inodo) == FALLO) return FALLO;

    // Comprobar permisos de lectura
    if ((inodo.permisos & 4) != 4) {
        fprintf(stderr, "no tienes permisos de lectura\n");
        return -1;
    }

    // Evitar leer más allá del tamaño lógico
    if (offset >= inodo.tamEnBytesLog) return 0;
    if (offset + nbytes > inodo.tamEnBytesLog)
        nbytes = inodo.tamEnBytesLog - offset;

    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    if (primerBL == ultimoBL) {
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        if (nbfisico != -1) {
            if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        } else {
            memset(buf_original, 0, nbytes); // hueco
        }
        nbytesLeidos = nbytes;
    } else {
        // Primer bloque
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        int tam = BLOCKSIZE - desp1;
        if (nbfisico != -1) {
            if (bread(nbfisico, buf_bloque) == -1) return -1;
            memcpy(buf_original, buf_bloque + desp1, tam);
        } else {
            memset(buf_original, 0, tam);
        }
        nbytesLeidos += tam;

        // Bloques intermedios 
        for (unsigned int i = primerBL + 1; i < ultimoBL; i++) {
            nbfisico = traducir_bloque_inodo(ninodo, i, 0);
            if (nbfisico != -1) {
                if (bread(nbfisico, buf_original + nbytesLeidos) == -1) return -1;
            } else {
                memset(buf_original + nbytesLeidos, 0, BLOCKSIZE);
            }
            nbytesLeidos += BLOCKSIZE;
        }

        // Ultimo bloque
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
        int resto = desp2 + 1;
        if (nbfisico != -1) {
            if (bread(nbfisico, buf_bloque) == -1) return -1;
            memcpy(buf_original + nbytesLeidos, buf_bloque, resto);
        } else {
            memset(buf_original + nbytesLeidos, 0, resto);
        }
        nbytesLeidos += resto;
    }

    // Actualizar atime
    inodo.atime = time(NULL);
    if (escribir_inodo(ninodo, &inodo) == -1) return -1;

    return nbytesLeidos;
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat) {
	struct inodo inodo;
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
	p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
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

 int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
	struct inodo inodo;
	int primerBL;
	if(leer_inodo(ninodo, &inodo) == FALLO) {
		fprintf(stderr, RED "Error al leer el inodo en mi_truncar_f\n" RESET);
		return FALLO;
	}

	if((inodo.permisos & 2) != 2) {
		fprintf(stderr, RED "Error, el inodo no tiene permisos de escritura\n" RESET);
		return FALLO;
	}

	//no se puede truncar mas alla del tamaño en bytes logicos del fichero/directorio
	if(nbytes > inodo.tamEnBytesLog) {
		fprintf(stderr, RED "Error, no se puede truncar más allá del tamaño lógico del fichero\n" RESET);
		return FALLO;
	}

	

	primerBL = (nbytes + BLOCKSIZE - 1) / BLOCKSIZE;

	//actualizar el mtime y ctime del inodo
	inodo.mtime = time(NULL);
	inodo.ctime = time(NULL);

	int liberados = liberar_bloques_inodo(primerBL, &inodo);

	inodo.numBloquesOcupados -= liberados;
	inodo.tamEnBytesLog = nbytes;

	//Salvar el inodo actualizado en disco
	if(escribir_inodo(ninodo, &inodo) == FALLO) {
		fprintf(stderr, RED "Error al escribir el inodo en mi_truncar_f\n" RESET);
		return FALLO;
	}

	return EXITO;

 }