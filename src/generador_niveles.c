#include "generador_niveles.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void limpiar_salto_linea(char *cadena)
{
    size_t len = strlen(cadena);
    if (len > 0 && cadena[len - 1] == '\n') {
        cadena[len - 1] = '\0';
    }
}

int procesar_cancion_y_metadatos(const char *ruta_audio, int id_nivel, int dificultad_deseada, MetadatosCancion *meta_salida)
{
    char cmd[1024];
    FILE *fp;

    snprintf(cmd, sizeof(cmd), "ffprobe -v error -show_entries format_tags=title -of default=noprint_wrappers=1:nokey=1 \"%s\"", ruta_audio);
    fp = popen(cmd, "r");
    if (fp) {
        if (fgets(meta_salida->titulo, sizeof(meta_salida->titulo), fp) != NULL) {
            limpiar_salto_linea(meta_salida->titulo);
        } else {
            strncpy(meta_salida->titulo, "Desconocido", sizeof(meta_salida->titulo));
        }
        pclose(fp);
    }

    snprintf(cmd, sizeof(cmd), "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"%s\"", ruta_audio);
    fp = popen(cmd, "r");
    if (fp) {
        char buffer_duracion[64] = {0};
        if (fgets(buffer_duracion, sizeof(buffer_duracion), fp) != NULL) {
            meta_salida->duracion = atof(buffer_duracion);
        } else {
            meta_salida->duracion = 0.0f;
        }
        pclose(fp);
    }

    snprintf(meta_salida->ruta_portada, sizeof(meta_salida->ruta_portada), "./niveles/portada_%d.jpg", id_nivel);
    snprintf(cmd, sizeof(cmd), "ffmpeg -v error -y -i \"%s\" -an -frames:v 1 \"%s\" 2>/dev/null", ruta_audio, meta_salida->ruta_portada);
    meta_salida->tiene_portada = (system(cmd) == 0);

    /* si es MP3 se crea un archivo ogg temporal */
    char ruta_analisis[512];
    strncpy(ruta_analisis, ruta_audio, sizeof(ruta_analisis));
    bool es_mp3 = (strstr(ruta_audio, ".mp3") != NULL || strstr(ruta_audio, ".MP3") != NULL);
    
    if (es_mp3) {
        snprintf(ruta_analisis, sizeof(ruta_analisis), "./niveles/temp_%d.ogg", id_nivel);
        snprintf(cmd, sizeof(cmd), "ffmpeg -v error -y -i \"%s\" -vn -c:a libvorbis -q:a 4 \"%s\"", ruta_audio, ruta_analisis);
        if (system(cmd) != 0) {
            game_log(LOG_ERROR, "Error convirtiendo MP3 a OGG temporal", ruta_audio);
            return -1;
        }
    }

    if (dificultad_deseada == 0) {
        for (int i = 1; i <= 3; i++) {
            snprintf(cmd, sizeof(cmd), "./generador_mapas \"%s\" %d > \"./niveles/nivel%d_diff%d.txt\"", ruta_analisis, i, id_nivel, i);
            if (system(cmd) != 0) {
                game_log(LOG_ERROR, "Error al invocar ./generador_mapas", cmd);
                return -1;
            }
        }
    } else if (dificultad_deseada >= 1 && dificultad_deseada <= 3) {
        snprintf(cmd, sizeof(cmd), "./generador_mapas \"%s\" %d > \"./niveles/nivel%d_diff%d.txt\"", ruta_analisis, dificultad_deseada, id_nivel, dificultad_deseada);
        if (system(cmd) != 0) {
            game_log(LOG_ERROR, "Error al invocar ./generador_mapas", cmd);
            return -1;
        }
    } else {
         game_log(LOG_ERROR, "Dificultad fuera de rango", 0);
         return -1;
    }

    /* limpieza del archivo temporal */
    if (es_mp3) {
        remove(ruta_analisis);
    }

    char ruta_meta[512];
    snprintf(ruta_meta, sizeof(ruta_meta), "./niveles/nivel%d_meta.txt", id_nivel);
    FILE *f_meta = fopen(ruta_meta, "w");
    if (f_meta) {
        fprintf(f_meta, "%s\n", meta_salida->titulo);
        fprintf(f_meta, "%f\n", meta_salida->duracion);
        fprintf(f_meta, "%s\n", meta_salida->ruta_portada);
        fprintf(f_meta, "%s\n", ruta_audio); /* Se inyecta la ruta origen del audio, NO el temporal */
        fclose(f_meta);
    } else {
        game_log(LOG_ERROR, "No se pudo escribir el archivo de metadatos", ruta_meta);
    }
    
    game_log(LOG_INFO, "Metadatos y niveles procesados exitosamente", meta_salida->titulo);
    return 0;
}
