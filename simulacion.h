#ifndef SIMULACION_H
#define SIMULACION_H

#include "directorios.h"
#include "bloques.h"

#define REGMAX 500000
#define NUMPROCESOS 100
#define NUMESCRITURAS 50

struct REGISTRO {
    time_t fecha;
    pid_t pid;
    int nEscritura;
    int nRegistro;
};

#endif