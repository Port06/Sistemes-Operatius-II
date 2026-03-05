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
				BREAK;
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
	//Leemos el superbloque para obtener la posicion del bloque de metadatos
	struct superbloque SB;
	if(bread(posSB, &SB) == FALLO) {
		fprintf(stderr, RED "Error al leer la estructura en SB\n" RESET);
		return FALLO;
	}

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

	if (bwrite(SB.posPrimerBloqueMB + nbloqueMB, bufferMB) == -1) {
		return FALLO;
	}

    return EXITO;
	
	
}

char leer_bit(unsigned int nbloque){
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

    //Se limpia el bloque de datos reservados
    unsigned char bufferCeros[BLOCKSIZE];
    memset(bufferCeros, 0, BLOCKSIZE);

    if (bwrite(nbloque, bufferCeros) == FALLO) {
        fprintf(stderr, RED "Error al limpiar el bloque reservado\n" RESET);
        return FALLO;
    }

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

	//Escribimos el bit del bloque que queremos liberar a 0
	if(escribir_bit(nbloque, 0) == FALLO) {
		fprintf(stderr, RED "Error al escribir el bit en liberar_bloque\n" RESET);
		return FALLO;
	}
	
	//Actualizamos la cantidad de bloques libres en el superbloque
	SB.cantBloquesLibres++;

	//Escribimos el superbloque actualizado
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

    //Se devuelve el numero del inodo reservado
    return posInodoReservado;
}

//Funcion para calcular el rango necessario de 0 a 3 (capacidad maxima) 
int obtener_nRangoBL (struct inodo *inodo, unsigned int nblogico, unsigned int *ptr) {


    if (nblogico<DIRECTOS) {  // <12
        *ptr:=inodo->punterosDirectos[nblogico];   
		return 0;  
	}
    else if (nblogico<INDIRECTOS0) {  // <268    
        *ptr:=inodo->punterosIndirectos[0] ;       
        return 1;
    }
    else if (nblogico<INDIRECTOS1 entonces) { // <65.804     
        *ptr:=inodo->punterosIndirectos[1];            
        return 2;
	}
    else if (nblogico<INDIRECTOS2 entonces) { // <16.843.020              
        *ptr:=inodo->punterosIndirectos[2];               
        return 3;
    }
    else {          
        *ptr:=0;            
        fprintf(stderr, RED "Error bloque logico fuera de rango\n" RESET);        
        return -1;  
    }        
};


funcion obtener_indice (nblogico: unsigned ent, nivel_punteros:ent) devolver ind:ent
si nblogico < DIRECTOS entonces devolver nblogico   //ej. nblogico=8
si_no si nblogico < INDIRECTOS0 entonces devolver nblogico - DIRECTOS   //ej. nblogico=204
si_no si nblogico < INDIRECTOS1 entonces   //ej. nblogico=30.004        
  si nivel_punteros = 2 entonces
     devolver (nblogico - INDIRECTOS0) / NPUNTEROS           
  si_no si nivel_punteros=1 entonces
     devolver (nblogico - INDIRECTOS0) % NPUNTEROS           
  fsi        
si_no si nblogico < INDIRECTOS2 entonces   //ej. nblogico=400.004           
  si nivel_punteros = 3 entonces
     devolver (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS)              
  si_no si nivel_punteros = 2 entonces      
     devolver ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS            
  si_no si nivel_punteros = 1 entonces    
     devolver ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS   
  fsi            
fsi
ffuncion


int traducir_bloque_inodo(unsigned int inodo, unsigned int nblogico, unsigned char reservar) {
	
	var
      ptr, ptr_ant, salvar_inodo:  unsigned ent  
      nRangoBL, nivel_punteros, indice:  ent  
      buffer[NPUNTEROS]: unsigned ent 
      inodo: struct inodo
   fvar
   ptr := 0, ptr_ant := 0, salvar_inodo := 0, indice:= 0
   leer_inodo(ninodo, &inodo)
   nRangoBL := obtener_nRangoBL(&inodo, nblogico, &ptr); //0:D, 1:I0, 2:I1, 3:I2
   nivel_punteros := nRangoBL //el nivel_punteros +alto es el que cuelga directamente del inodo

   si nRangoBL=0 entonces //Caso punteros Directos

      si ptr=0 //no existe bloque de datos
         si reservar=0 entonces devolver -1 fsi  //error ∄ bloque -> no imprimir error por pantalla!!! 
         ptr = reservar_bloque() //de datos
         inodo.numBloquesOcupados++
         inodo.ctime = time(NULL)
         inodo.punterosDirectos[nblogico] := ptr //asignamos la direción del bl. de datos en el inodo
         salvar_inodo := 1
      fsi

   si_no //Caso de punteros Indirectos
      mientras nivel_punteros>0 hacer //iterar para cada nivel de punteros indirectos
          si ptr=0 entonces //no cuelgan bloques de punteros
             si reservar=0 entonces devolver -1 fsi// error ∄ bloque ->  no imprimir error por pantalla!!!
             //reservar bloques de punteros y crear enlaces desde el  inodo hasta el bloque de datos
             ptr := reservar_bloque() //de punteros                  
             inodo.numBloquesOcupados++
             inodo.ctime = time(NULL) //fecha actual
             salvar_inodo := 1
             si nivel_punteros = nRangoBL entonces  //el bloque cuelga directamente del inodo
                inodo.punterosIndirectos[nRangoBL-1] := ptr 
             si_no   //el bloque cuelga de otro bloque de punteros
                buffer[indice] := ptr 
                bwrite(ptr_ant, buffer)  //salvamos en el dispositivo el buffer de punteros modificado           
             fsi
             memset(buffer, 0, BLOCKSIZE) //ponemos a 0 todos los punteros del buffer 
         si_no //ptr!=0
            bread(ptr, buffer) //leemos del dispositivo el bloque de punteros ya existente
         fsi
         indice := obtener_indice(nblogico, nivel_punteros)
         ptr_ant := ptr //guardamos el puntero actual
         ptr := buffer[indice] // y lo desplazamos al siguiente nivel 
         nivel_punteros--   
      fmientras   //al salir de este bucle ya estamos al nivel de datos
		si ptr=0 entonces 
         si reservar=0 entonces devolver -1 fsi  //bloque inexistente -> no imprimir error por pantalla!!!
         ptr := reservar_bloque() //de datos                  
         inodo.numBloquesOcupados++
         inodo.ctime = time(NULL) //fecha actual
         salvar_inodo := 1
         buffer[indice] := ptr //asignamos la dirección del bloque de datos en el buffer
         bwrite(ptr_ant, buffer) //salvamos en el dispositivo el buffer de punteros modificado 
      fsi
   fsi  //fin caso Indirectos

   //salvar el inodo si se han hecho cambios y se desea no tener un big lock al usar semáforos
   si salvar_inodo entonces escribir_inodo(ninodo, &inodo) fsi
   devolver ptr //nº de bloque físico correspondiente al bloque de datos lógico, nblogico
ffuncion

	
}; 