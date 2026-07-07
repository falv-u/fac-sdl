#ifndef GENERADOR_NIVELES_H
#define GENERADOR_NIVELES_H

#include <stdbool.h>

typedef struct {
    char titulo[256];
    float duracion;
    char ruta_portada[512];
    bool tiene_portada;
} MetadatosCancion;

int procesar_cancion_y_metadatos(const char *ruta_audio, int id_nivel, int dificultad_deseada, MetadatosCancion *meta_salida);

#endif
