#ifndef VERIFICACION_H
#define VERIFICACION_H

#include "simulacion.h"

#define BUFFER_REGISTROS 64

struct INFORMACION {
    int pid;
    unsigned int nEscrituras;   // validadas
    struct REGISTRO PrimeraEscritura;
    struct REGISTRO UltimaEscritura;
    struct REGISTRO MenorPosicion;
    struct REGISTRO MayorPosicion;
};

#endif