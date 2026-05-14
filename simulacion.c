// Archivo simulacion.c

#include "simulacion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>

#define DEBUG 1   // poner a 0 para la entrega final

int acabados = 0;


// Función enterrador
void reaper() {

    pid_t ended;

    signal(SIGCHLD, reaper);

    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0) {

        acabados++;
    }
}


int main(int argc, char **argv) {

    // Comprobación sintaxis
    if (argc != 2) {

        fprintf(stderr,
                "Uso: %s <disco>\n",
                argv[0]);

        return FALLO;
    }


    // Asociar señal SIGCHLD
    signal(SIGCHLD, reaper);


    // Montar dispositivo
    if (bmount(argv[1]) == FALLO) {

        fprintf(stderr,
                "Error al montar el dispositivo\n");

        return FALLO;
    }


    // Crear nombre directorio simulación
    char camino_simulacion[100];

    time_t ahora = time(NULL);

    struct tm *tm = localtime(&ahora);

    sprintf(camino_simulacion,
            "/simul_%04d%02d%02d%02d%02d%02d/",
            tm->tm_year + 1900,
            tm->tm_mon + 1,
            tm->tm_mday,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec);


    // Mostrar nombre simulación
    printf("*** SIMULACIÓN DE %d PROCESOS REALIZANDO "
           "CADA UNO %d ESCRITURAS ***\n",
           NUMPROCESOS,
           NUMESCRITURAS);

    printf("%s\n", camino_simulacion);


    // Crear directorio simulación
    if (mi_creat(camino_simulacion, 6) < 0) {

        fprintf(stderr,
                "Error al crear directorio simulación\n");

        bumount();

        return FALLO;
    }


    // Crear procesos
    for (int proceso = 1;
         proceso <= NUMPROCESOS;
         proceso++) {

        pid_t pid = fork();


        // Error fork
        if (pid < 0) {

            fprintf(stderr,
                    "Error al crear proceso hijo\n");

            break;
        }


        // Código hijo
        if (pid == 0) {

            // El hijo remonta el dispositivo
            if (bmount(argv[1]) == FALLO) {

                fprintf(stderr,
                        "Error al montar dispositivo hijo\n");

                exit(EXIT_FAILURE);
            }


            // Crear directorio del proceso
            char dir_proceso[200];

            sprintf(dir_proceso,
                    "%sproceso_%d/",
                    camino_simulacion,
                    getpid());


            if (mi_creat(dir_proceso, 6) < 0) {

                fprintf(stderr,
                        "Error al crear directorio proceso\n");

                bumount();

                exit(EXIT_FAILURE);
            }


            // Crear fichero prueba.dat
            char fichero_prueba[300];

            sprintf(fichero_prueba,
                    "%sprueba.dat",
                    dir_proceso);


            if (mi_creat(fichero_prueba, 6) < 0) {

                fprintf(stderr,
                        "Error al crear prueba.dat\n");

                bumount();

                exit(EXIT_FAILURE);
            }


            // Inicializar semilla aleatoria
            srand(time(NULL) + getpid());


            // Escrituras
            for (int nescritura = 1;
                 nescritura <= NUMESCRITURAS;
                 nescritura++) {

                struct REGISTRO registro;

                registro.fecha = time(NULL);

                registro.pid = getpid();

                registro.nEscritura = nescritura;

                registro.nRegistro = rand() % REGMAX;


                // Calcular offset
                unsigned int offset =
                    registro.nRegistro *
                    sizeof(struct REGISTRO);


                // Escribir registro
                if (mi_write(fichero_prueba,
                             &registro,
                             offset,
                             sizeof(struct REGISTRO)) < 0) {

                    fprintf(stderr,
                            "Error escritura registro\n");

                    bumount();

                    exit(EXIT_FAILURE);
                }


#if DEBUG
                fprintf(stderr,
                        "[simulacion.c → Escritura %d en %s]\n",
                        nescritura,
                        fichero_prueba);
#endif


                // Esperar 0,05 segundos
                usleep(50000);
            }


            // Mensaje final proceso
            fprintf(stderr,
                    "[Proceso %d: Completadas %d escrituras "
                    "en %s]\n",
                    proceso,
                    NUMESCRITURAS,
                    fichero_prueba);


            // Desmontar dispositivo hijo
            bumount();


            // MUY IMPORTANTE
            exit(EXIT_SUCCESS);
        }


        // Padre espera 0,15 segundos
        usleep(150000);
    }


    // Esperar hijos
    while (acabados < NUMPROCESOS) {

        pause();
    }


    // Desmontar dispositivo padre
    bumount();

    return EXITO;
}