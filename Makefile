CC=gcc
CFLAGS=-c -g -Wall -std=gnu17
LDFLAGS=-pthread

SOURCES=\
bloques.c mi_mkfs.c ficheros_basico.c ficheros.c leer_sf.c escribir.c leer.c \
truncar.c permitir.c directorios.c mi_mkdir.c mi_chmod.c mi_ls.c mi_link.c \
mi_escribir.c mi_cat.c mi_stat.c mi_rm.c simulacion.c verificacion.c semaforo_mutex_posix.c

OBJS=$(SOURCES:.c=.o)

PROGRAMS=mi_mkfs leer_sf escribir leer truncar permitir mi_mkdir mi_chmod mi_ls mi_link mi_escribir mi_cat mi_stat mi_rm simulacion verificacion

all: $(PROGRAMS)

# Regla genérica de ejecutables
%: %.o bloques.o ficheros_basico.o ficheros.o directorios.o semaforo_mutex_posix.o
	$(CC) $(LDFLAGS) $^ -o $@

# compilación .o
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(PROGRAMS) disco* ext*