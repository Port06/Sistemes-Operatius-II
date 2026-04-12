CC=gcc
CFLAGS=-c -g -Wall -std=gnu17
LDFLAGS=   # Si necesitas pthread, descomenta: -pthread

SOURCES=bloques.c mi_mkfs.c ficheros_basico.c leer_sf.c ficheros.c escribir.c leer.c permitir.c truncar.c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o
PROGRAMS=mi_mkfs leer_sf escribir leer permitir truncar

all: $(PROGRAMS)

# Regla genérica para enlazar cualquier programa
$(PROGRAMS): %: %.o $(LIBRARIES)
	$(CC) $(LDFLAGS) $^ -o $@

# Regla para compilar cualquier .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o *~ $(PROGRAMS) disco* ext*
