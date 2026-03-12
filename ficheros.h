#include "ficheros_basico.h"
#include <time.h>

struct STAT {     // comprobar que ocupa 128 bytes haciendo un sizeof(inodo)!!!
   unsigned char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero)
   unsigned char permisos; // Permisos (lectura y/o escritura y/o ejecución)
   time_t atime; // Fecha y hora del último acceso a datos (contenido)
   time_t mtime; // Fecha y hora de la última modificación de datos (contenido)
   time_t ctime; // Fecha y hora de la última modificación del inodo
   time_t btime; // Fecha y hora de creación del inodo (birth)
   unsigned int nlinks;             // Cantidad de enlaces de entradas en directorio
   unsigned int tamEnBytesLog;      // Tamaño en bytes lógicos (EOF)
   unsigned int numBloquesOcupados; // Cantidad de bloques ocupados zona de datos

};