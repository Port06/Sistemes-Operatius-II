//Archivo fichero_basico.c

#include "ficheros_basico.h"
#include <time.h>
#include <limits.h>

int cont_bread = 0, cont_bwrite = 0;


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
	//Se calcula el numero de bloques para el array inodos en fraccion
    int tamAI = (ninodos * INODOSIZE) / BLOCKSIZE;

	//Luego de incrementa el numero de bolques en 1 si la division no es exacta
    if (((ninodos * INODOSIZE) % BLOCKSIZE) != 0) {
        tamAI++;
    }

    return tamAI;
}

//Metodo que inizializa a los bits del los metadatos
int initMB() {
    struct superbloque SB;

    // Leemos el superbloque
    if (bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
        return FALLO;
    }
	cont_bread++;

    // Marcamos como ocupados todos los bloques de metadatos
    for (unsigned int i = 0; i < SB.posPrimerBloqueDatos; i++) {
        escribir_bit(i, 1);
    }

    // Corregimos la cantidad de bloques libres
    SB.cantBloquesLibres = SB.totBloques - SB.posPrimerBloqueDatos;

    // Guardamos el superbloque actualizado
    if (bwrite(posSB, &SB) == FALLO) {
        fprintf(stderr, RED "Error al escribir el superbloque\n" RESET);
        return FALLO;
    }
	cont_bwrite++;

    return EXITO;
}

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
	cont_bwrite++;

	return EXITO;
}

//Metodo para inizializar los inodos libres
int initAI(){
	struct superbloque SB;
	if (bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	cont_bread++;

	struct inodo inodos [BLOCKSIZE/INODOSIZE];
	int ContInodos = SB.posPrimerInodoLibre+1;
	for(int i=SB.posPrimerBloqueAI; i<=SB.posUltimoBloqueAI; i++){
		if(bread(i, &inodos) == FALLO) {
			fprintf(stderr, RED "Error al leer la estructura en AI\n" RESET);
			return FALLO;
		}
		cont_bread++;
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
		cont_bwrite++;
	
	
}
return EXITO;
}

int escribir_bit(unsigned int nbloque, unsigned int bit){
	//Leemos el superbloque para obtener la posicion del bloque de metadatos
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	cont_bread++;

	//Calculamos la posicion del byte y el bit dentro del bloque de metadatos
	int posbyteMB = nbloque / 8;
	int posbit = nbloque % 8;
	int nbloqueMB = posbyteMB / BLOCKSIZE;
	int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

	unsigned char bufferMB[BLOCKSIZE];

	if (bread(nbloqueabs, bufferMB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	cont_bread++;

	//Calculamos la posicion del byte dentro del bloque de metadatos
	int posbyte = posbyteMB % BLOCKSIZE;
	unsigned char mascara = 128; //10000000
	mascara >>= posbit; //Desplazamos la mascara a la derecha segun el numero de bit

	//Se escribe en la posicion alcanzada el valor del binario deseado
	if(bit == 1){
		bufferMB[posbyte] |= mascara; //Pone a 1 el bit, con la operacion logica de OR
	}else{
		bufferMB[posbyte] &= ~mascara; //Pone a 0 el bit, con la operacion logica de AND
	}

	if (bwrite(SB.posPrimerBloqueMB + nbloqueMB, bufferMB) == FALLO) {
		return FALLO;
	}
	cont_bwrite++;
	
    return EXITO;
	
	
}

char leer_bit(unsigned int nbloque){
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	cont_bread++;

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
	cont_bread++;

	int posbyte = posbyteMB % BLOCKSIZE;
	unsigned char mascara = 128; // 10000000
	mascara >>= posbit;// desplazamiento de bits a la derecha, los que indique posbit
	mascara &= bufferMB[posbyte]; // operador AND para bits
	mascara >>= (7 - posbit); // desplazamiento de bits a la derecha 
    // para dejar el 0 o 1 en el extremo derecho y leerlo en decimal
	
	return mascara;
}

int reservar_bloque() {
	struct superbloque SB;

    //Se lee del superbloque
    if (bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED "Error al leer el superbloque\n" RESET);
        return FALLO;
    }
	cont_bread++;

    //Se comprueba si quedan bloques libres
    if (SB.cantBloquesLibres == 0) {
        fprintf(stderr, RED "No quedan bloques libres\n" RESET);
        return FALLO; //En caso negativo no se puede reservar un bloque
    }

    unsigned char bufferMB[BLOCKSIZE];
    unsigned char bufferAux[BLOCKSIZE];

    // bufferAux lleno de 1s (255)
    memset(bufferAux, 255, BLOCKSIZE);

    int nbloqueMB = 0;

    //Se busca el primer bloque del megabyte con un bit a 0
    while (nbloqueMB < (SB.posUltimoBloqueMB - SB.posPrimerBloqueMB + 1)) {
        if (bread(SB.posPrimerBloqueMB + nbloqueMB, bufferMB) == FALLO) {
            fprintf(stderr, RED "Error al leer bloque del MB\n" RESET);
            return FALLO;
        }
		cont_bread++;

        if (memcmp(bufferMB, bufferAux, BLOCKSIZE) != 0) {
            break; // este bloque tiene al menos un 0
        }

        nbloqueMB++;
    }

    //Se localiza el primer byte con un bit a 0
    int posbyte = 0;
    while (bufferMB[posbyte] == 255) {
        posbyte++;
    }

    //Se localiza el primer bit a 0 dentro del byte
    unsigned char mascara = 128; // 10000000
    int posbit = 0;

	//Localiacion del bit dentro del byte
    while (bufferMB[posbyte] & mascara) {
        bufferMB[posbyte] <<= 1;
        posbit++;
    }

    //Se calcula el numero de bloque fisico
    int nbloque = (nbloqueMB * BLOCKSIZE + posbyte) * 8 + posbit;

    //Se marca el bloque como ocupado en el megabyte
    if (escribir_bit(nbloque, 1) == FALLO) {
        fprintf(stderr, RED "Error al escribir bit en reservar_bloque\n" RESET);
        return FALLO;
    }

    //Se actualiza el superbloque
    SB.cantBloquesLibres--;
    if (bwrite(posSB, &SB) == FALLO) {
        fprintf(stderr, RED "Error al escribir el superbloque\n" RESET);
        return FALLO;
    }
	cont_bwrite++;

    //Se limpia el bloque de datos reservados
    unsigned char bufferCeros[BLOCKSIZE];
    memset(bufferCeros, 0, BLOCKSIZE);

    if (bwrite(nbloque, bufferCeros) == FALLO) {
        fprintf(stderr, RED "Error al limpiar el bloque reservado\n" RESET);
        return FALLO;
    }
	cont_bwrite++;

    //Se devuelve el numero del bloque reservado
    return nbloque;
}


int liberar_bloque(unsigned int nbloque) {
	//leemos el superbloque para obtener la posicion del bloque de metadatos
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	cont_bread++;

	//Escribimos el bit del bloque que queremos liberar a 0
	if(escribir_bit(nbloque, 0) == FALLO) {
		fprintf(stderr, RED "Error al escribir el bit en liberar_bloque\n" RESET);
		return FALLO;
	}
	
	// Imprimir el bloque liberado
	fprintf(stderr, "LIBERADO BLOQUE: %u\n", nbloque);
	
	//Actualizamos la cantidad de bloques libres en el superbloque
	SB.cantBloquesLibres++;

	//Escribimos el superbloque actualizado
	if (bwrite(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al escribir la estructura en SB\n" RESET);
		return FALLO;
	}
	cont_bwrite++;


	return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo) {
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	cont_bread++;

	int nbloqueAI = (ninodo * INODOSIZE) / BLOCKSIZE;
	int nbloqueabs = SB.posPrimerBloqueAI + nbloqueAI;

	struct inodo inodos[BLOCKSIZE/INODOSIZE];

	if(bread(nbloqueabs, inodos) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en AI\n" RESET);
		return FALLO;
	}
	cont_bread++;

	int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);
	inodos[posinodo] = *inodo;

	if(bwrite(nbloqueabs, inodos) == FALLO) {
		fprintf(stderr, RED "Error al escribir la estructura en AI\n" RESET);
		return FALLO;
	}
	cont_bwrite++;

	return EXITO;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo) {
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}
	cont_bread++;

	//Calculamos el bloque del array de inodos donde se encuentra el inodo que queremos leer (igual que en escribir_inodo)
	int nbloqueAI = (ninodo * INODOSIZE) / BLOCKSIZE;
	int nbloqueabs = SB.posPrimerBloqueAI + nbloqueAI;

	struct inodo inodos[BLOCKSIZE/INODOSIZE];
	if(bread(nbloqueabs, inodos) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en AI\n" RESET);
		return FALLO;
	}
	cont_bread++;

	//Calculamos la posicion del inodo dentro del bloque de inodos
	int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);
	*inodo = inodos[posinodo];

	return EXITO;
	
	
}

//Metodo de reserva de inodo
int reservar_inodo(unsigned char tipo, unsigned char permisos) {
    struct superbloque SB;
    struct inodo inodo;
    unsigned int posInodoReservado;

    //Se lee el superbloque
    if (bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED "Error al leer el superbloque\n" RESET);
        return FALLO;
    }
	cont_bread++;
	
	//Se comprueba de que la lista de inodos este libres
	if (SB.posPrimerInodoLibre == UINT_MAX) {
		fprintf(stderr, RED "Lista de inodos libres vacía\n" RESET);
		return FALLO;
	}

    //Se comprueba si hay inodos libres
    if (SB.cantInodosLibres == 0) {
        fprintf(stderr, RED "No quedan inodos libres\n" RESET);
        return FALLO;
    }

    //Se almazena la posicion del primer inodo libre
    posInodoReservado = SB.posPrimerInodoLibre;

    //Se lee ese inodo para saber cual es el siguiente libre
    if (leer_inodo(posInodoReservado, &inodo) == FALLO) {
        fprintf(stderr, RED "Error al leer el inodo libre\n" RESET);
        return FALLO;
    }

    //Se actualiza la lista de inodos libres
    SB.posPrimerInodoLibre = inodo.punterosDirectos[0];

    //Se inicializa el inodo reservado
    inodo.tipo = tipo;
    inodo.permisos = permisos;
    inodo.nlinks = 1;
    inodo.tamEnBytesLog = 0;
    inodo.atime = time(NULL);
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.numBloquesOcupados = 0;

    //Inicializacion punteros directos
    for (int i = 0; i < 12; i++) {
        inodo.punterosDirectos[i] = 0; //0 para evitar basura
    }

    //Inicializacion punteros indirectos
    for (int i = 0; i < 3; i++) {
        inodo.punterosIndirectos[i] = 0; //0 para evitar basura
    }

    //Se escribe el inodo inicializado
    if (escribir_inodo(posInodoReservado, &inodo) == FALLO) {
        fprintf(stderr, RED "Error al escribir el inodo reservado\n" RESET);
        return FALLO;
    }

    //Se actualiza el superbloque
    SB.cantInodosLibres--;
    if (bwrite(posSB, &SB) == FALLO) {
        fprintf(stderr, RED "Error al escribir el superbloque\n" RESET);
        return FALLO;
    }
	cont_bwrite++;

    //Se devuelve el numero del inodo reservado
    return posInodoReservado;
}

//Funcion para calcular el rango necessario de 0 a 3 (capacidad maxima) 
int obtener_nRangoBL (struct inodo inodo, unsigned int nblogico, unsigned int *ptr) {


    if (nblogico<DIRECTOS) {  // <12
        *ptr=inodo.punterosDirectos[nblogico];   
		return 0;  
	}
    else if (nblogico<INDIRECTOS0) {  // <268    
        *ptr=inodo.punterosIndirectos[0] ;       
        return 1;
    }
    else if (nblogico<INDIRECTOS1) { // <65.804     
        *ptr=inodo.punterosIndirectos[1];            
        return 2;
	}
    else if (nblogico<INDIRECTOS2) { // <16.843.020              
        *ptr=inodo.punterosIndirectos[2];               
        return 3;
    }
    else {          
        *ptr=0;            
        fprintf(stderr, RED "Error bloque logico fuera de rango\n" RESET);        
        return FALLO;  
    }        
};

//Funcion para obtener el valor entero del indice
int obtener_indice (unsigned int nblogico , int nivel_punteros) {
    if (nblogico < DIRECTOS) {  //ej. nblogico=8
        return nblogico;
    } else if (nblogico < INDIRECTOS0) {
        return nblogico - DIRECTOS; //ej. nblogico=204
    } else if (nblogico < INDIRECTOS1) { //ej. nblogico=30.004   
        if (nivel_punteros == 2) {
            return (nblogico - INDIRECTOS0) / NPUNTEROS;
        } else if (nivel_punteros==1) {
            return  (nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    } else if (nblogico < INDIRECTOS2) {  //ej. nblogico=400.004     
        if(nivel_punteros == 3) {
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        } else if(nivel_punteros == 2) {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;
        } else if (nivel_punteros == 1) {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS;   
        }
    }     
    return FALLO;
}


int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, unsigned char reservar) {

	//Definicion de variables
    unsigned int ptr, ptr_ant, salvar_inodo;
    int nRangoBL, nivel_punteros, indice;   
    unsigned int buffer[NPUNTEROS]; 
    struct inodo inodo;

	ptr = 0;
	ptr_ant = 0;
	salvar_inodo = 0;
	indice = 0;
	leer_inodo(ninodo, &inodo);
	nRangoBL = obtener_nRangoBL(inodo, nblogico, &ptr); //0:D, 1:I0, 2:I1, 3:I2
	nivel_punteros = nRangoBL; //El nivel_punteros +alto es el que cuelga directamente del inodo

    if (nRangoBL == 0) { //Caso punteros Directos

        if (ptr == 0) { //No existe bloque de datos
			if (reservar == 0) return FALLO; //Error bloque a no imprimir por pantalla 
			ptr = reservar_bloque(); //Reservacion de bloque de datos
			fprintf(stderr, "reservado bloque datos (directo): %u\n", ptr);
			inodo.numBloquesOcupados++;
			inodo.ctime = time(NULL);
			inodo.punterosDirectos[obtener_indice(nblogico, 0)] = ptr; //Se asigna la direción del bloque de datos en el inodo
			salvar_inodo = 1;
        };
    }  
    else { //Caso de punteros Indirectos
        while (nivel_punteros>0) { //Iterar para cada nivel de punteros indirectos
            if (ptr == 0) { //No cuelgan bloques de punteros
				if (reservar == 0) return FALLO; //Error bloque a no imprimir por pantalla
				//Reservar bloques de punteros y crear enlaces desde el inodo hasta el bloque de datos
				ptr = reservar_bloque(); //Reservacion de bloque de punteros
				fprintf(stderr, "reservado bloque de punteros (nivel %d): %u\n", nivel_punteros, ptr);				
				inodo.numBloquesOcupados++;
				inodo.ctime = time(NULL); //Fecha actual
				salvar_inodo = 1;
				if (nivel_punteros == nRangoBL) {  //El bloque cuelga directamente del inodo 
					inodo.punterosIndirectos[nRangoBL-1] = ptr;
				}
				else { //El bloque cuelga de otro bloque de punteros
					buffer[indice] = ptr; 
					bwrite(ptr_ant, buffer);  //Salvamos en el dispositivo el buffer de punteros modificado 
					cont_bwrite++;
				};
				memset(buffer, 0, BLOCKSIZE); //Ponemos a 0 todos los punteros del buffer 
			}
			else { //ptr!=0
				bread(ptr, buffer); //Leemos del dispositivo el bloque de punteros ya existente
				cont_bread++;
			};
			indice = obtener_indice(nblogico, nivel_punteros);
			ptr_ant = ptr; //Guardamos el puntero actual
			ptr = buffer[indice]; //Lo desplazamos al siguiente nivel 
			nivel_punteros--;   
	    } //Al salir de este bucle ya estamos al nivel de datos

		if (ptr == 0) {
			if (reservar == 0) return FALLO;  //Bloque inexistente a no imprimir por pantalla
			ptr = reservar_bloque(); //Reservacion de bloque de datos      
			fprintf(stderr, "reservado bloque datos: %u\n", ptr);
			inodo.numBloquesOcupados++;
			inodo.ctime = time(NULL); //Fecha actual
			salvar_inodo = 1;
			buffer[indice] = ptr; //Asignamos la dirección del bloque de datos en el buffer
			bwrite(ptr_ant, buffer); //Salvamos en el dispositivo el buffer de punteros modificado 
			cont_bwrite++;
		};
	}; //Fin caso Indirectos

	//Salvar el inodo si se han hecho cambios y se desea no tener un big lock al usar semáforos
	if (salvar_inodo) escribir_inodo(ninodo, &inodo);
    return ptr; //Numero de bloque físico correspondiente al bloque de datos lógico, nblogico	
};

int liberar_inodo(unsigned int ninodo) {
	
	// El primer bloque siempre empieza en 0
	unsigned int primerBL = 0;
	
	// Definicion struct de inodo
	struct inodo inodo;
	leer_inodo(ninodo, &inodo);

	// Obtener el numero de bloques inodo liberados
	int liberados = liberar_bloques_inodo(primerBL, &inodo);
	
	inodo.numBloquesOcupados -= liberados; // debería quedar a 0
	inodo.tipo = 'l';
	inodo.tamEnBytesLog = 0;
	
	// Definicion struct de superbloque
	struct superbloque SB;
	bread(posSB, &SB);
	cont_bread++;

	// El inodo liberado apunta al antiguo primero libre
	inodo.punterosDirectos[0] = SB.posPrimerInodoLibre;

	// Ahora el primero libre es este
	SB.posPrimerInodoLibre = ninodo;	
	SB.cantInodosLibres++;
	
	// Escribir el superbloque
	bwrite(posSB, &SB);
	cont_bwrite++;

	// Actualizar tiempo de cambio
	inodo.ctime = time(NULL);
	
	// Escribir el inodo antes de devolverlo
	escribir_inodo(ninodo, &inodo);
	
	// Imprimir lecturas y escrituras
	fprintf(stderr, "Total breads: %d\n", cont_bread);
	fprintf(stderr, "Total bwrites: %d\n", cont_bwrite);

	return ninodo;	
}

int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo) {
	
	// libera los bloques de datos e índices recorriendo los punteros desde el inodo hacia las hojas (BLs  datos)
    
    unsigned int nivel_punteros = 0, nBL = primerBL, ultimoBL, ptr = 0; 
    int nRangoBL = 0, liberados = 0, eof = 0;

    if (inodo->tamEnBytesLog == 0) return 0; // el fichero está vacío

	// obtenemos el último bloque lógico del inodof
	if (inodo->tamEnBytesLog % BLOCKSIZE == 0) {		
    ultimoBL = (inodo->tamEnBytesLog - 1) / BLOCKSIZE;
	} else {
		ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE;
	}

    nRangoBL = obtener_nRangoBL(*inodo, nBL, &ptr);

    if  (nRangoBL == 0) {
        liberados += liberar_directos(&nBL, ultimoBL, inodo, &eof);
    }

    while (!eof) {
        nRangoBL = obtener_nRangoBL(*inodo, nBL, &ptr);
        nivel_punteros = nRangoBL;

        int resultado = liberar_indirectos_recursivo(&nBL, primerBL, ultimoBL, inodo, nRangoBL, nivel_punteros, &ptr, &eof);
        liberados = liberados + resultado;
    }

    return liberados;
};


// Función para liberar bloques directos
int liberar_directos(unsigned int *nBL, unsigned int ultimoBL, struct inodo *inodo, int *eof) {

    int liberados = 0;

    while (*nBL < DIRECTOS && !(*eof)) {

        if (inodo->punterosDirectos[*nBL] != 0) {
            liberar_bloque(inodo->punterosDirectos[*nBL]);
            inodo->punterosDirectos[*nBL] = 0;
            liberados++;
        }

        (*nBL)++;  

        if (*nBL > ultimoBL) *eof = 1; // Fin del archivo lógico
    }

    return liberados;	
};

// Función para liberar bloques indirectos de manera recursiva
int liberar_indirectos_recursivo(unsigned int *nBL, unsigned int primerBL, unsigned int ultimoBL, 
	struct inodo *inodo, int nRangoBL, unsigned int nivel_punteros, unsigned int *ptr, int *eof) {
    
    int liberados = 0;
    int modificado = 0;   // 1 si se ha puesto alguna entrada del bloque a 0

    unsigned int bloquePunteros[NPUNTEROS];
    unsigned int bufferCeros[NPUNTEROS];

    memset(bufferCeros, 0, BLOCKSIZE);
    
    if (*ptr == 0) {
		switch (nRangoBL) { // Saltos al valer 0 un puntero del inodo según nivel
           case 1: *nBL = INDIRECTOS0; break;
           case 2: *nBL = INDIRECTOS1; break;
           case 3: *nBL = INDIRECTOS2; break;
		}
		return liberados;
    }

    // Leemos el bloque de punteros del dispositivo si no está ya en memoria
    int indice_inicial = obtener_indice(*nBL, nivel_punteros);

    if (indice_inicial == 0 || *nBL == primerBL) { //solo leemos bloque si no ya estaba cargado previamente
        if (bread(*ptr, bloquePunteros) == FALLO) return FALLO;  
		cont_bread++;
    }         

    // Recorremos las entradas del bloque de punteros desde indice_inicial.
    for (int i = indice_inicial; i < NPUNTEROS && !(*eof); i++) { 

        if (bloquePunteros[i] != 0) {              

            if (nivel_punteros == 1) {
                liberar_bloque(bloquePunteros[i]);  // Bloque de datos
                bloquePunteros[i] = 0;
                modificado = 1;
                liberados++;
                (*nBL)++;
			} else { // Llamada recursiva para explorar el nivel siguiente de punteros hacia los datos
                unsigned int ptr_antes = bloquePunteros[i];

                liberados += liberar_indirectos_recursivo(nBL, primerBL, ultimoBL, inodo, 
														nRangoBL, nivel_punteros - 1, &bloquePunteros[i], eof);

				if (bloquePunteros[i] != ptr_antes) modificado = 1;
			}

		} else { // *ptr=0, los BLs que dependen de esta entrada no existen
            switch (nivel_punteros) {  // Saltos al valer 0 un puntero según nivel
              case 1: (*nBL)++; break;
              case 2: (*nBL) += NPUNTEROS; break;
              case 3: (*nBL) += NPUNTEROS * NPUNTEROS; break;
            }
		}

		if (*nBL > ultimoBL) *eof = 1;  // Comprobamos si hemos llegado al fin del archivo
    }

    // Comprobamos si tras procesar las entradas quedo el bloque vacío
    if (memcmp(bloquePunteros, bufferCeros, BLOCKSIZE) == 0) {
		// Si vacio el llamante liberara *ptr y pondrá la entrada a 0
        liberar_bloque(*ptr);  //de punteros
        *ptr = 0;  // Ponemos a 0 el puntero que apuntaba al bloque liberado
        liberados++;
    } else if (modificado == 1) { // No vacio pero si modificado se escriben los cambios en disco
        bwrite(*ptr, bloquePunteros);
		cont_bwrite++;
    }

    return liberados;
}