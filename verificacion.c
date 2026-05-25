#include "verificacion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

// Configuración de verbosidad
static int verbose = 0;  // 0 = silencioso, 1 = solo info importante, 2 = todo

void set_verbose(int level) {
    verbose = level;
}

// Utilidades internas
static void asegurar_barra_final(char *ruta) {
    size_t len = strlen(ruta);
    if (len == 0) return;

    if (ruta[len - 1] != '/') {
        ruta[len] = '/';
        ruta[len + 1] = '\0';
    }
}

static int extraer_pid_nombre(const char *nombre, int *pid) {
    const char *prefijo = "proceso_";
    size_t len_prefijo = strlen(prefijo);

    if (strncmp(nombre, prefijo, len_prefijo) != 0) {
        return FALLO;
    }

    *pid = atoi(nombre + len_prefijo);
    return EXITO;
}

static void formatear_fecha(time_t fecha, char *buffer, size_t tam) {
    struct tm *tm_info = localtime(&fecha);

    if (tm_info == NULL) {
        snprintf(buffer, tam, "(fecha invalida)");
        return;
    }

    strftime(buffer, tam, "%a %d-%m-%Y %H:%M:%S", tm_info);
}

static int append_fmt(const char *camino, unsigned int *offset, const char *fmt, ...) {
    char buffer[1024];  // Aumentado para manejar líneas más largas
    va_list ap;

    va_start(ap, fmt);
    int n = vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    if (n < 0 || n >= (int)sizeof(buffer)) {
        if (verbose >= 1) fprintf(stderr, "Error: buffer insuficiente para formatear\n");
        return FALLO;
    }

    int escritos = mi_write(camino, buffer, *offset, (unsigned int)n);
    if (escritos != n) {
        if (verbose >= 1) fprintf(stderr, "Error: solo se escribieron %d de %d bytes\n", escritos, n);
        return FALLO;
    }

    *offset += (unsigned int)escritos;
    return EXITO;
}

static void inicializar_informacion(struct INFORMACION *info, int pid) {
    memset(info, 0, sizeof(struct INFORMACION));
    info->pid = pid;
}

static void actualizar_informacion(struct INFORMACION *info, struct REGISTRO *reg) {
    if (info->nEscrituras == 0) {
        info->PrimeraEscritura = *reg;
        info->UltimaEscritura   = *reg;
        info->MenorPosicion     = *reg;
        info->MayorPosicion     = *reg;
    } else {
        if (reg->nEscritura < info->PrimeraEscritura.nEscritura) {
            info->PrimeraEscritura = *reg;
        }
        if (reg->nEscritura > info->UltimaEscritura.nEscritura) {
            info->UltimaEscritura = *reg;
        }
        if (reg->nRegistro < info->MenorPosicion.nRegistro) {
            info->MenorPosicion = *reg;
        }
        if (reg->nRegistro > info->MayorPosicion.nRegistro) {
            info->MayorPosicion = *reg;
        }
    }

    info->nEscrituras++;
}

static int escribir_informe_proceso(const char *camino_informe, unsigned int *offset, const struct INFORMACION *info) {
    char fecha[64];

    if (append_fmt(camino_informe, offset, "PID: %d\n", info->pid) == FALLO) return FALLO;
    if (append_fmt(camino_informe, offset, "Numero de escrituras: %u\n", info->nEscrituras) == FALLO) return FALLO;

    formatear_fecha(info->PrimeraEscritura.fecha, fecha, sizeof(fecha));
    if (append_fmt(camino_informe, offset, "Primera Escritura\t %d\t %d\t %s\n",
                   info->PrimeraEscritura.nEscritura,
                   info->PrimeraEscritura.nRegistro,
                   fecha) == FALLO) return FALLO;

    formatear_fecha(info->UltimaEscritura.fecha, fecha, sizeof(fecha));
    if (append_fmt(camino_informe, offset, "Ultima Escritura\t %d\t %d\t %s\n",
                   info->UltimaEscritura.nEscritura,
                   info->UltimaEscritura.nRegistro,
                   fecha) == FALLO) return FALLO;

    formatear_fecha(info->MenorPosicion.fecha, fecha, sizeof(fecha));
    if (append_fmt(camino_informe, offset, "Menor Posicion\t %d\t %d\t %s\n",
                   info->MenorPosicion.nEscritura,
                   info->MenorPosicion.nRegistro,
                   fecha) == FALLO) return FALLO;

    formatear_fecha(info->MayorPosicion.fecha, fecha, sizeof(fecha));
    if (append_fmt(camino_informe, offset, "Mayor Posicion\t %d\t %d\t %s\n\n",
                   info->MayorPosicion.nEscritura,
                   info->MayorPosicion.nRegistro,
                   fecha) == FALLO) return FALLO;

    return EXITO;
}

// Programa principal
int main(int argc, char **argv) {
    // Verificar argumentos
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Uso: %s <nombre_dispositivo> <directorio_simulacion> [-q]\n", argv[0]);
        fprintf(stderr, "  -q : modo silencioso (suprime mensajes no esenciales)\n");
        return FALLO;
    }
    
    // Opción silenciosa
    if (argc == 4 && strcmp(argv[3], "-q") == 0) {
        set_verbose(0);
        // Redirigir stderr a null temporalmente (solo para la biblioteca)
        // Esto no es elegante pero funciona si no puedes modificar la biblioteca
        freopen("/dev/null", "w", stderr);
    } else {
        set_verbose(1);
    }

    char dir_sim[1024];
    char informe_path[1024];

    strncpy(dir_sim, argv[2], sizeof(dir_sim) - 1);
    dir_sim[sizeof(dir_sim) - 1] = '\0';
    asegurar_barra_final(dir_sim);

    if (verbose >= 1) printf("Montando dispositivo %s...\n", argv[1]);
    
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar el dispositivo\n");
        return FALLO;
    }

    if (verbose >= 1) printf("dir_sim: %s\n", dir_sim);

    struct STAT stat_dir;
    if (mi_stat(dir_sim, &stat_dir) == FALLO) {
        fprintf(stderr, "Error al obtener stat del directorio de simulacion\n");
        bumount();
        return FALLO;
    }

    unsigned int numentradas = stat_dir.tamEnBytesLog / sizeof(struct entrada);
    
    if (verbose >= 1) {
        printf("numentradas: %u NUMPROCESOS: %d\n", numentradas, NUMPROCESOS);
    }

    if (numentradas != NUMPROCESOS) {
        fprintf(stderr, "ERROR: el numero de entradas del directorio de simulacion no coincide con NUMPROCESOS\n");
        bumount();
        return FALLO;
    }

    snprintf(informe_path, sizeof(informe_path), "%sinforme.txt", dir_sim);

    if (mi_creat(informe_path, 6) == FALLO) {
        fprintf(stderr, "Error al crear informe.txt\n");
        bumount();
        return FALLO;
    }

    unsigned int offset_informe = 0;
    struct entrada ent;
    struct INFORMACION info;
    int procesos_procesados = 0;
    int errores = 0;

    for (unsigned int i = 0; i < numentradas; i++) {
        unsigned int off_entrada = i * sizeof(struct entrada);

        if (mi_read(dir_sim, &ent, off_entrada, sizeof(struct entrada)) != (int)sizeof(struct entrada)) {
            fprintf(stderr, "Error al leer la entrada %u del directorio de simulacion\n", i);
            errores++;
            continue;
        }

        int pid_proceso;
        if (extraer_pid_nombre(ent.nombre, &pid_proceso) == FALLO) {
            if (verbose >= 2) fprintf(stderr, "Omitiendo entrada inválida: %s\n", ent.nombre);
            continue;
        }

        char camino_prueba[1024];
        snprintf(camino_prueba, sizeof(camino_prueba), "%s%s/prueba.dat", dir_sim, ent.nombre);

        struct STAT stat_fichero;
        if (mi_stat(camino_prueba, &stat_fichero) == FALLO) {
            fprintf(stderr, "Error al obtener stat de %s\n", camino_prueba);
            errores++;
            continue;
        }

        inicializar_informacion(&info, pid_proceso);

        unsigned int tam_logico = stat_fichero.tamEnBytesLog;
        unsigned int offset = 0;
        struct REGISTRO buffer[BUFFER_REGISTROS];

        if (verbose >= 2) printf("Procesando PID %d, tamaño archivo: %u bytes\n", pid_proceso, tam_logico);

        while (offset < tam_logico) {
            unsigned int bytes_a_leer = sizeof(buffer);
            if (tam_logico - offset < bytes_a_leer) {
                bytes_a_leer = tam_logico - offset;
            }

            int leidos = mi_read(camino_prueba, buffer, offset, bytes_a_leer);
            if (leidos < 0) {
                fprintf(stderr, "Error al leer %s (offset=%u)\n", camino_prueba, offset);
                errores++;
                break;
            }

            if (leidos == 0) {
                break;
            }

            unsigned int nregs = (unsigned int)leidos / sizeof(struct REGISTRO);

            for (unsigned int j = 0; j < nregs; j++) {
                if (buffer[j].pid == pid_proceso) {
                    actualizar_informacion(&info, &buffer[j]);
                }
            }

            offset += (unsigned int)leidos;
        }

        procesos_procesados++;

        if (verbose >= 1) {
            printf("PID: %d\n", info.pid);
            printf("Numero de escrituras: %u\n", info.nEscrituras);

            char fecha[64];
            formatear_fecha(info.PrimeraEscritura.fecha, fecha, sizeof(fecha));
            printf("Primera Escritura\t %d\t %d\t %s\n",
                   info.PrimeraEscritura.nEscritura,
                   info.PrimeraEscritura.nRegistro,
                   fecha);

            formatear_fecha(info.UltimaEscritura.fecha, fecha, sizeof(fecha));
            printf("Ultima Escritura\t %d\t %d\t %s\n",
                   info.UltimaEscritura.nEscritura,
                   info.UltimaEscritura.nRegistro,
                   fecha);

            formatear_fecha(info.MenorPosicion.fecha, fecha, sizeof(fecha));
            printf("Menor Posicion\t %d\t %d\t %s\n",
                   info.MenorPosicion.nEscritura,
                   info.MenorPosicion.nRegistro,
                   fecha);

            formatear_fecha(info.MayorPosicion.fecha, fecha, sizeof(fecha));
            printf("Mayor Posicion\t %d\t %d\t %s\n\n",
                   info.MayorPosicion.nEscritura,
                   info.MayorPosicion.nRegistro,
                   fecha);
        }

        if (escribir_informe_proceso(informe_path, &offset_informe, &info) == FALLO) {
            fprintf(stderr, "Error al escribir el informe del proceso %d\n", pid_proceso);
            errores++;
        }
    }

    if (verbose >= 1) {
        printf("\n--- Resumen ---\n");
        printf("Procesos procesados: %d\n", procesos_procesados);
        printf("Errores: %d\n", errores);
        printf("Informe guardado en: %s\n", informe_path);
    }

    bumount();
    
    return (errores == 0) ? EXITO : FALLO;
}