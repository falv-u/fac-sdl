#ifndef COMMONS_H
#define COMMONS_H

#include <stdbool.h>

/* SDL libs */
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "SDL_rect.h"
#include "SDL_video.h"
#include "SDL_events.h"
#include "SDL_render.h"
#include <SDL_ttf.h>
#include <SDL_audio.h>
/* Macros */
#define TARGET_FPS      60

#define LARGO_PANTALLA 720
#define ANCHO_PANTALLA 1280
#define LOG_LEVEL LOG_DEBUG /* Maquina de estado */
#define FPS 30
#define FRAME_TARGET_TIME (1000/FPS)

#define ESCALADO_1 3

#define TILE_64 64

extern int last_frame_time;


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
	SDL_Rect rectangulo;
} entidad_rec_simple ;

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

	ElementosMenu elementos[3];
} menu_principal_recursos;

typedef struct {
    bool keyUp;
    bool keyDown;
} InputState;

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

typedef struct {
    TIPO_NOTA tipo;
	 ESTADO_NOTA estado;
    SDL_Rect hitbox; 
    int carril;       
} Nota;

typedef struct {
    Nota *notas;
    int total_notas;
} MapaCancion;

SDL_Window *crear_ventana();

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
	
} eventos_globales;

typedef struct {
	bool single_player;
	bool niveles_aleatorios;
	int dificultad;
} tipo_partida;
void assets_load(void);

void Pantalla_Completa(SDL_Window *ventana);

ESTADO_ACTUAL menu_principal(eventos_globales *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu);
ESTADO_ACTUAL juego_principal(eventos_globales *ev_gl, SDL_Event *evento);
void generar_rectangulo(entidad_rec_simple *rectangulo);
void update(float delta_time, eventos_globales *ev_gl, menu_principal_recursos *rec_menu);
ESTADO_ACTUAL seleccionar_niveles(void);
#endif
