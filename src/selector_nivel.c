#include "commons.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <SDL_ttf.h>
#include "generador_niveles.h"
void listar_niveles(void);
void generar_playlist_aleatoria(eventos_globales *ev_gl, int dificultad_deseada);

void sincronizar_biblioteca_audio(void);

void listar_niveles(void)
{
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char ruta_completa[512];
    char time_srt[64];
    FILE *archivo_salida;
    const char *carpeta_niveles = "./niveles/";
    const char *ruta_archivo_salida = "./niveles_db.txt";
    sincronizar_biblioteca_audio();
    
    int numero_nivel = -1;
    int dificultad = -1;

    dir = opendir(carpeta_niveles);
    archivo_salida = fopen(ruta_archivo_salida, "w");

    if (archivo_salida == NULL) {
        game_log(LOG_ERROR, "No se pudo crear el archivo de base de datos de niveles", ruta_archivo_salida);
        return;
    }

    if (dir == NULL) {
        game_log(LOG_ERROR, "Carpeta de niveles no encontrada", NULL);
        fclose(archivo_salida);
        return;
    }

    fprintf(archivo_salida, "Numero_Nivel;Dificultad;Ruta_Completa;Fecha_Modificacion\n");

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        /* Parsea archivos tipo: nivel1_diff2.txt */
        if (sscanf(entry->d_name, "nivel%d_diff%d.txt", &numero_nivel, &dificultad) == 2) {
            snprintf(ruta_completa, sizeof(ruta_completa), "%s%s", carpeta_niveles, entry->d_name);

            if (stat(ruta_completa, &file_stat) == 0) {
                strftime(time_srt, sizeof(time_srt), "%d-%m-%Y %H:%M", localtime(&file_stat.st_mtime));
                fprintf(archivo_salida, "%d;%d;%s;%s\n", numero_nivel, dificultad, ruta_completa, time_srt);
            }
        }
    }

    closedir(dir);
    fclose(archivo_salida);
}

void generar_playlist_aleatoria(eventos_globales *ev_gl, int dificultad_deseada)
{
    listar_niveles();

    FILE *db = fopen("./niveles_db.txt", "r");
    if (!db) {
        game_log(LOG_ERROR, "No se pudo leer niveles_db.txt", 0);
        ev_gl->playlist.modo_playlist = false;
        return;
    }

    char linea[512];
    char rutas_filtradas[128][512];
    int ids_filtrados[128]; /* Nuevo: Almacena el ID único de la canción */
    int total_filtradas = 0;

    if (!fgets(linea, sizeof(linea), db)) {
        fclose(db);
        return;
    }

    while (fgets(linea, sizeof(linea), db) && total_filtradas < 128) {
        int num, diff;
        char ruta[512];
        char fecha[64];
        if (sscanf(linea, "%d;%d;%[^;];%s", &num, &diff, ruta, fecha) >= 3) {
            /* Si la dificultad es 4, ignoramos el filtro y cargamos todo */
            if (dificultad_deseada == 4 || diff == dificultad_deseada) {
                strncpy(rutas_filtradas[total_filtradas], ruta, 512);
                ids_filtrados[total_filtradas] = num;
                total_filtradas++;
            }
        }
    }
    fclose(db);

    if (total_filtradas == 0) {
        game_log(LOG_WARN, "No se encontraron canciones", 0);
        ev_gl->playlist.cantidad = 0;
        ev_gl->playlist.modo_playlist = false;
        return;
    }

    ev_gl->playlist.cantidad = (total_filtradas < MAX_CANCIONES_PLAYLIST) ? total_filtradas : MAX_CANCIONES_PLAYLIST;
    ev_gl->playlist.actual = 0;
    ev_gl->playlist.modo_playlist = true;

    int indices_usados[MAX_CANCIONES_PLAYLIST] = {-1, -1, -1, -1};
    int ids_usados[MAX_CANCIONES_PLAYLIST] = {-1, -1, -1, -1}; 
    for (int i = 0; i < ev_gl->playlist.cantidad; i++) {
        int idx_elegido;
        int repetido;
        int iteraciones_seguridad = 0;
        do {
            repetido = 0;
            idx_elegido = rand() % total_filtradas;
            for (int j = 0; j < i; j++) {
                if (indices_usados[j] == idx_elegido || ids_usados[j] == ids_filtrados[idx_elegido]) {
                    repetido = 1;
                    break;
                }
            }
            iteraciones_seguridad++;
            if (iteraciones_seguridad > 1000) break;
        } while (repetido);

        indices_usados[i] = idx_elegido;
        ids_usados[i] = ids_filtrados[idx_elegido];
        strncpy(ev_gl->playlist.rutas[i], rutas_filtradas[idx_elegido], 512);

        char ruta_meta[512];
        snprintf(ruta_meta, sizeof(ruta_meta), "./niveles/nivel%d_meta.txt", ids_filtrados[idx_elegido]);
        FILE *fm = fopen(ruta_meta, "r");
        
        if (fm) {
            fgets(ev_gl->playlist.titulos[i], 256, fm);
            ev_gl->playlist.titulos[i][strcspn(ev_gl->playlist.titulos[i], "\n")] = 0; 
            
            char buf_dur[64];
            fgets(buf_dur, 64, fm);
            ev_gl->playlist.duraciones[i] = atof(buf_dur);

            fgets(ev_gl->playlist.portadas[i], 512, fm);
            ev_gl->playlist.portadas[i][strcspn(ev_gl->playlist.portadas[i], "\n")] = 0;
            fclose(fm);
        } else {
            strcpy(ev_gl->playlist.titulos[i], "Desconocido");
            ev_gl->playlist.duraciones[i] = 0.0f;
            strcpy(ev_gl->playlist.portadas[i], "");
        }
    }
}

ESTADO_ACTUAL seleccionar_niveles(eventos_globales *ev_gl, menu_principal_recursos *rec_menu)
{
    SDL_SetRenderDrawColor(ev_gl->renderizado, 10, 15, 30, 255);
    SDL_RenderClear(ev_gl->renderizado);

    if (!rec_menu->fuente) {
        return ESTADO_MENU;
    }

    SDL_Color color_blanco = {255, 255, 255, 255};
    SDL_Color color_seleccionado = {255, 215, 0, 255}; /* Dorado */

    const char *opciones[4] = {"1. FACIL (Dificultad 1)", "2. NORMAL (Dificultad 2)", "3. DIFICIL (Dificultad 3)", "4. ALEATORIO TOTAL (Sin restriccion)"};

    /* Renderizar un título simple */
    SDL_Surface *surf_tit = TTF_RenderText_Solid(rec_menu->fuente, "SELECCIONA DIFICULTAD DE LA PARTIDA", color_blanco);
    if (surf_tit) {
        SDL_Texture *tex_tit = SDL_CreateTextureFromSurface(ev_gl->renderizado, surf_tit);
        SDL_Rect rect_tit = { (ANCHO_PANTALLA / 2) - (surf_tit->w / 2), 80, surf_tit->w, surf_tit->h };
        SDL_RenderCopy(ev_gl->renderizado, tex_tit, NULL, &rect_tit);
        SDL_DestroyTexture(tex_tit);
        SDL_FreeSurface(surf_tit);
    }
    for (int i = 0; i < 4; i++) {
        SDL_Color color_actual = (ev_gl->opcion_dificultad == i) 
                                 ? (SDL_Color){255, 215, 0, 255}  // Amarillo (Dorado)
                                 : (SDL_Color){255, 255, 255, 255}; // Blanco

        SDL_Surface *surf_opt = TTF_RenderText_Solid(rec_menu->fuente, opciones[i], color_actual);
        
        if (surf_opt) {
            SDL_Texture *tex_opt = SDL_CreateTextureFromSurface(ev_gl->renderizado, surf_opt);
            
            SDL_Rect rect_opt = { 
                (ANCHO_PANTALLA / 2) - (surf_opt->w / 2), 
                240 + (i * 80), 
                surf_opt->w, 
                surf_opt->h 
            };

            SDL_RenderCopy(ev_gl->renderizado, tex_opt, NULL, &rect_opt);
            
            SDL_DestroyTexture(tex_opt);
            SDL_FreeSurface(surf_opt);
        }
    }

    SDL_RenderPresent(ev_gl->renderizado);
    return ESTADO_SELECT_NIVELES;
}
void sincronizar_biblioteca_audio(void)
{
    DIR *dir;
    struct dirent *entry;
    int id_nivel = 1;
    const char *carpeta_musica = "./assets/music/";

    dir = opendir(carpeta_musica);
    if (!dir) {
        game_log(LOG_WARN, "No se encontro el directorio de musica", carpeta_musica);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        /* Filtrar solo archivos .ogg */
        if (strstr(entry->d_name, ".ogg")) {
            char ruta_audio[512];
            char ruta_test_nivel[512];
            snprintf(ruta_audio, sizeof(ruta_audio), "%s%s", carpeta_musica, entry->d_name);
            
            /* Verificamos si el nivel 1 de este ID ya existe para no regenerar en cada inicio */
            snprintf(ruta_test_nivel, sizeof(ruta_test_nivel), "./niveles/nivel%d_diff1.txt", id_nivel);
            FILE *f_test = fopen(ruta_test_nivel, "r");
            
            if (!f_test) {
                game_log(LOG_INFO, "Nuevo audio detectado. Generando mapas y metadatos...", entry->d_name);
                MetadatosCancion meta;
                /* 0 indica generar las 3 dificultades */
                procesar_cancion_y_metadatos(ruta_audio, id_nivel, 0, &meta); 
            } else {
                fclose(f_test);
            }
            id_nivel++;
        }
    }
    closedir(dir);
}
