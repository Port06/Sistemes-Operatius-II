#include <directorios.h>


// Variables cache para no bubscar mismas direcciones al escribir repetidamente en el mismo fichero
static struct {
    char camino[1024];
    int p_inodo;
} 
UltimaEntradaEscritura = {"", -1};

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
	if (*camino != '/') {
		fprintf(stderr, RED "Error al escribir la ruta, debe empezar con '/'" RESET);
		return FALLO;
	};
	
	// Parte de fichero de la ruta de directorio
	// Esta parte puede anidar otra ruta de directorio
	char *segundo = strchr(camino + 1, '/');
	
	if (segundo != NULL) {	// Si se encuentra '/' es un directorio
	
		// Cantidad de caracteres a calcular para concluir el array con el caracter de final de array
		int longitud = segundo - (camino + 1);
		
		strncpy(inicial, camino + 1, segundo - (camino + 1));
		inicial[longitud] = '\0'; // Añadir caracter centinela

		strcpy(final, segundo);  // Copiar el resultado con el carater de conclusionde array
		
		*tipo = 'd';
		
	} else { // Si es un fichero
		// Se mantiene intacto
		strcpy(inicial, camino + 1);
		final[0] = '\0';

		*tipo = 'f';
	}
	
	return EXITO;
};


int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {
	
    struct entrada entrada;
    struct inodo inodo_dir;

    char inicial[TAMNOMBRE];
    char final[strlen(camino_parcial) + 1];
    char tipo;

    int cant_entradas_inodo, num_entrada_inodo = 0;


    if (strcmp(camino_parcial, "/") == 0) { // Camino_parcial es “/”
       *p_inodo = SB.posInodoRaiz;  // Nuestra raiz siempre estará asociada al inodo 0
       *p_entrada = 0;
       return 0;
    };
 
    // Extraer camino
    if (extraer_camino(camino_parcial, inicial, final, &tipo) == FALLO) {
        return ERROR_CAMINO_INCORRECTO;
    }
      
	// Buscamos la entrada cuyo nombre se encuentra en inicial
	leer_inodo( *p_inodo_dir, &inodo_dir);
	
    if (!(inodo_dir.permisos & 4)) { // Permiso lectura
        return ERROR_PERMISO_LECTURA;
    }


    // Inicializar el buffer de lectura con 0s
    // El buffer de lectura ha de ser un array de las entradas que caben en un bloque,
	
    // Calcular número de entradas
    cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada);

    num_entrada_inodo = 0;
	
	// Buscar entrada
    if (cant_entradas_inodo > 0) {
		mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada));
	 
        while ((num_entrada_inodo < cant_entradas_inodo) &&
               (strcmp(inicial, entrada.nombre) != 0)) {

            num_entrada_inodo++;

            if (num_entrada_inodo < cant_entradas_inodo) {
                mi_read_f(*p_inodo_dir, &entrada,
                          num_entrada_inodo * sizeof(struct entrada),
                          sizeof(struct entrada));
            }
        }
    }


    if ((num_entrada_inodo == cant_entradas_inodo) || (cant_entradas_inodo > 0 && strcmp(inicial, entrada.nombre) != 0)) {
		// La entrada no existe
        switch(reservar) {
            case 0: // Modo consulta. Como no existe retornamos error
                return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
			case 1:  // Modo escritura: Creamos la entrada en el directorio referenciado por *p_inodo_dir
                // Si es fichero no permitir escritura
                if (inodo_dir.tipo == 'f') {
                   return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
                }	
				
                // Si es directorio comprobar que tiene permiso de escritura
                if (!(inodo_dir.permisos & 2)) { // Permiso escritura
                    return ERROR_PERMISO_ESCRITURA;
                } else {
					strcpy(entrada.nombre, inicial);
                    if (tipo == 'd') {
                        if (strcmp(final, "/") == 0) {
                            entrada.ninodo = reservar_inodo('d', permisos);
						} else {
							return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
						}
                    } else { // Es un fichero
                       entrada.ninodo = reservar_inodo('f', permisos);       
                    }
					
                    // Escribir entrada al final
					if (mi_write_f(*p_inodo_dir, &entrada,
								   cant_entradas_inodo * sizeof(struct entrada),
								   sizeof(struct entrada)) == FALLO) {

						if (entrada.ninodo != -1) {
							liberar_inodo(entrada.ninodo);
						}
						return FALLO;
					}
				}  
			break;
        }
	}


    if (strcmp(final, "") == 0 || strcmp(final, "/") == 0) {
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1)) {
			// Modo escritura y la entrada ya existe
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
		
        // Cortamos la recursividad
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
		
        return EXITO;		
    } else {
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }	
};

int mi_creat(const char *camino, unsigned char permisos) {
    unsigned int p_inodo_dir = 0; // Raíz
    unsigned int p_inodo;
    unsigned int p_entrada;

    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos);

    if (error < 0) {
        mostrar_error_buscar_entrada(error); // Opcional
        return FALLO;
    }

    return EXITO;
};

int mi_chmod(const char *camino, unsigned char permisos) {
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo;
    unsigned int p_entrada;

    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos);

    if (error < 0) return FALLO;

    return mi_chmod_f(p_inodo, permisos);
};

int mi_stat(const char *camino, struct STAT *p_stat) {
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo;
    unsigned int p_entrada;

    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);

    if (error < 0) return FALLO;

    return mi_stat_f(p_inodo, p_stat);
};

int mi_dir(const char *camino, char *buffer) {

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo;
    unsigned int p_entrada;

    struct inodo inodo;
    struct entrada entrada;

    buffer[0] = '\0';

    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if (error < 0) return FALLO;

    leer_inodo(p_inodo, &inodo);

    // Comprobar que es directorio
    if (inodo.tipo != 'd') {
        return FALLO;
    }

    // Comprobar permisos lectura
    if (!(inodo.permisos & 4)) {
        return FALLO;
    }

    int nentradas = inodo.tamEnBytesLog / sizeof(struct entrada);

    for (int i = 0; i < nentradas; i++) {
        mi_read_f(p_inodo, &entrada, i * sizeof(struct entrada), sizeof(struct entrada));

        strcat(buffer, entrada.nombre);
        strcat(buffer, "\n"); // o '|'
    }

    return nentradas;
};

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes) {

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    // Si es el mismo camino no buscamos otra vez
	if (strcmp(UltimaEntradaEscritura.camino, camino) == 0) {
		p_inodo = UltimaEntradaEscritura.p_inodo;
	} else {
		int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
		if (error < 0) return error;

		strcpy(UltimaEntradaEscritura.camino, camino);
		UltimaEntradaEscritura.p_inodo = p_inodo;
	}

    // Delegar a capa de ficheros
    return mi_write_f(p_inodo, buf, offset, nbytes);
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes) {

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    // Buscar fichero
    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);

    if (error < 0) return error;

    // Delegar a capa de ficheros
    return mi_read_f(p_inodo, buf, offset, nbytes);
}

int mi_link(const char *camino1, const char *camino2){
    int p_inodo_dir1 = 0;
    int p_inodo1;
    int p_entrada1;
    int p_inodo_dir2 = 0;
    int p_inodo2;   
    int p_entrada2;

    //obtener el numero de inodo del camino1
    int error = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, 0, 0);
    if (error < 0) return error;
    //comprobar que tiene permisos de lectura
    struct inodo inodo1;
    leer_inodo(p_inodo1, &inodo1);
    if (!(inodo1.permisos & 4)) return ERROR_PERMISO_LECTURA;
    
    //Camino 1 y camino 2 deben referirse a ficheros
    // No se permite el enlace a directorios para evitar que se creen ciclos en el grafo.
    if (inodo1.tipo != 'f') return ERROR_NO_ES_FICHERO;

    //La entrada del camino2 no debe existir, la hemos de crear con buscar_entrada con reservar = 1 y permisos 6 (lectura y escritura)
    error = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, 1, 6);
    if (error < 0) return error;

    //leemos la entrada creada de camino2, o sea la entrada p_entrada2 de p_inodo_dir2
    struct entrada entrada;
    mi_read_f(p_inodo_dir2, &entrada, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada));

    //Creamos el enlace, asociamos a esta entrada el mismo inodo que al asociado a la entrada del camino1, es decir p_inodo1
    entrada.ninodo = p_inodo1;

    //Escribimos la entrada modificada en p_inodo_dir2
    if (mi_write_f(p_inodo_dir2, &entrada, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada)) == FALLO) {
        liberar_inodo(entrada.ninodo); // Liberar el inodo creado para camino2
        return FALLO;
    }

    //Liberamos el inodo que se ha asociado a la entrada creada, p_inodo2.
    liberar_inodo(p_inodo2);

    //Incrementamos la cantidad de enlaces (nlinks) de p_inodo1, actualizamos el ctime y lo salvamos.
    inodo1.nlinks++;
    inodo1.ctime = time(NULL);
    escribir_inodo(p_inodo1, &inodo1);

    return EXITO;
}

int mi_unlink(const char *camino){
    
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo;
    unsigned int p_entrada;
    //Comprobar que la entrada existe y obtener p_entrada y p_inodo con la funcion buscar_entrada con reservar = 0
    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if (error < 0) return error;

    //leer el inodo, 
    struct inodo inodo;
    leer_inodo(p_inodo, &inodo);
    if(inodo.tamEnBytesLog == 0) {
        fprintf(stderr, RED "Error: el fichero ya está vacío\n" RESET);
        return FALLO;
    }
    //comprobamos el numero de entradas que tiene
    int nentradas = inodo.tamEnBytesLog/sizeof(struct entrada);

    if(p_entrada ==  nentradas -1){
        //truncar el inodo a su tamaño menos el tamaño de una entrada
        mi_truncar_f(p_inodo, inodo.tamEnBytesLog - sizeof(struct entrada));
    }else{
        //sobrescribir la entrada a eliminar con la última entrada del directorio
        struct entrada ultima_entrada;
        mi_read_f(p_inodo_dir, &ultima_entrada, (nentradas - 1) * sizeof(struct entrada), sizeof(struct entrada));
        if (mi_write_f(p_inodo_dir, &ultima_entrada, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) == FALLO) {
            return FALLO;
        }
        //truncar el inodo a su tamaño menos el tamaño de una entrada
        mi_truncar_f(p_inodo, inodo.tamEnBytesLog - sizeof(struct entrada));

    }

    p_inodo.nlinks--; // Decrementar el número de enlaces del inodo
    if(p_inodo.nlinks == 0) {
        liberar_inodo(p_inodo); // Liberar el inodo si no tiene enlaces
    } else {
        inodo.ctime = time(NULL); // Actualizar el tiempo de cambio
        escribir_inodo(p_inodo, &inodo); // Guardar los cambios en el inodo
    }


    
}