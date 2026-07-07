#ifndef COMMONS_H
#define COMMONS_H
/* standar headers */
#include <stdbool.h>

/* SDL libs */
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "SDL_gamecontroller.h"
#include "SDL_mixer.h"
#include "SDL_rect.h"
#include "SDL_video.h"
#include "SDL_events.h"
#include "SDL_render.h"
#include <SDL_ttf.h>
#include <SDL_audio.h>

/* Libs propias */
/*(ninguna por ahora)*/

/* Macros */

#define MAX_CANCIONES_PLAYLIST 4

/* Delta Time */
#define FPS 30
#define TARGET_FPS      60
#define FRAME_TARGET_TIME (1000/FPS)

#define Y_JUEZ 600.0f
#define Y_SPAWN -64.0f
#define TIEMPO_APROXIMACION 1.5f /* La flecha tarda 1.5s en caer desde que aparece */

extern int last_frame_time;

/* renderizado */
#define LARGO_PANTALLA 720
#define ANCHO_PANTALLA 1280
#define ESCALADO_1 3

#define TILE_64 64

SDL_Window *crear_ventana();

/* log.h */
#define LOG_LEVEL LOG_DEBUG /* Maquina de estado */

/* MENU */

typedef struct {
    int src_x, src_y;  // Coordenadas de origen 1024x1024
    int dest_x, dest_y;// Coordenadas de destino en la pantalla
    int ancho, alto;   // Tamaño del bloque  64x64
    int escala;        // Factor de escalado
} ElementosMenu;

typedef struct {
	/* menu_principal */
	SDL_Rect jugar;
	SDL_Rect opciones;
	SDL_Rect salir;
	TTF_Font *fuente;

	SDL_Texture* textura_titulo;
	int titulo_w, titulo_h;
	SDL_Texture *sprites;

	float scroll_camino;
	float scroll_edificios;

	float vel_camino;

	float vel_edificios;
	//SDL_Surface *surfaceTexto;
	//SDL_Texture *texturaTexto;
	SDL_Color color1;
	SDL_Color color2;
	SDL_Color color3;
	
	Mix_Music *musica_fondo;
	Mix_Chunk *sfx_opcion;
	int opcion;
} menu_principal_recursos;

/*--------------------------------------------------------------*/
/* Logica de juego */
typedef enum {
	NOTA_SIMPLE,
	NOTA_LARGA
} TIPO_NOTA;

typedef enum {
	NOTA_ESPERANDO,   /* sigue bajando */
	NOTA_MANTENIDA,   /* el jugador continua presionando */
	NOTA_GOLPEADA,    /* acertada */
	NOTA_FALLADA      /* pasa de largo */
} ESTADO_NOTA;

/* TODO: carriles de notas
typedef enum {
} carriles;
*/

typedef struct {
	float tiempo_golpe;  /* l momento exacto en que la base de la nota toca al juez */
	float duracion;      /* si es 0.0f, es nota normal. Si es > 0.0f, es tecla larga */
	int carril;          /* 0 a 3 (Jugador 1) | 4 a 7 (Jugador 2) */
	float y_actual;      /* Posición calculada de la base */
	bool activa;
	bool siendo_presionada; /* TRUE si el jugador la está manteniendo pisada */
} Nota;

typedef struct {
	SDL_Keycode teclas_p1[4];
	SDL_Keycode teclas_p2[4];
	bool teclas_aleatorias; /* Bandera que lee el parser del nivel.txt */
	Nota *arreglo_notas;
	int total_notas;
	int notas_pasadas;
} MapaCancion;

typedef struct {
	bool single_player;
	bool niveles_aleatorios;
	int dificultad;
} tipo_partida;

/* ---------------------------------------------- */

/* eventos y estados globables (requeridos por todo el juego) */
typedef enum {
	ESTADO_MENU,
	ESTADO_PAUSA,
	ESTADO_CARGA,
	ESTADO_SELECT_TIPO_PARTIDA,
	ESTADO_SELECT_NIVELES,
	ESTADO_JUEGO,
	ESTADO_GAMEOVER,
	ESTADO_SALIR,
} ESTADO_ACTUAL;

typedef struct {
    char rutas[MAX_CANCIONES_PLAYLIST][512];
    char titulos[MAX_CANCIONES_PLAYLIST][256];      
    float duraciones[MAX_CANCIONES_PLAYLIST];        
    char portadas[MAX_CANCIONES_PLAYLIST][512];
    char rutas_audio[MAX_CANCIONES_PLAYLIST][512];
    int cantidad;          
    int actual;            
    bool modo_playlist;    
} PlaylistDificultad;

typedef struct {
	SDL_Window *ventana;
	SDL_Renderer *renderizado;
	bool corriendo;
	int last_frame_time;
	ESTADO_ACTUAL estado_juego;

	float tiempo_juego;
	int indice_nota_actual;

	menu_principal_recursos rec_menu;
	float iris_radius;
	bool is_iris_fading_out;
	MapaCancion mapa_actual;

	PlaylistDificultad playlist;
	Mix_Music *musica_nivel_actual;
  int opcion_dificultad;
	
	SDL_GameController *mando_p1;
	SDL_GameController *mando_p2;
} eventos_globales;


/* -------------------------------------------------- */


/* primitivas */

/* renderizado en pantalla */
void Pantalla_Completa(SDL_Window *ventana);
void update(float delta_time, eventos_globales *ev_gl, menu_principal_recursos *rec_menu);
void assets_load(void);

/* primitivas de ESTADO_JUEGO*/
ESTADO_ACTUAL menu_principal(eventos_globales *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu);
ESTADO_ACTUAL seleccionar_niveles(eventos_globales *ev_gl, menu_principal_recursos *rec_menu);
void generar_playlist_aleatoria(eventos_globales *ev_gl, int dificultad_deseada);
ESTADO_ACTUAL juego_principal(eventos_globales *ev_gl, SDL_Event *evento);

/* primitivas de selector de niveles */
MapaCancion cargar_nivel(const char *ruta_archivo);
MapaCancion cargar_nivel_desde_lista(int indice_elegido);

#endif
