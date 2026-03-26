
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

// Prototipos de funciones de nivel 5
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
int mi_chmod_f(unsigned int ninodo, unsigned char permisos);