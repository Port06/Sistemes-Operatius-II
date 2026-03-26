#include <directorios.h>

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