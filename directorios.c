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
		inicial[longitud] = '\0';

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
	
	
	
};