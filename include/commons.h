#ifndef COMMONS_H
#define COMMONS_H

#include <stdbool.h>
#include "log.h"

/* SDL libs */
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
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

/* ================================================================
   ESTRUCTURAS
   ================================================================ */
typedef struct {
	SDL_Rect rectangulo;
} entidad_rec_simple ;

typedef struct {
	/* menu_principal */
	SDL_Rect jugar;
	SDL_Rect opciones;
	SDL_Rect salir;
	TTF_Font *fuente;

	SDL_Texture *domo;
	SDL_Texture *camino;
	SDL_Texture *edificios;
	SDL_Texture *coche;

	float scroll_camino;
   float scroll_edificios;
    
   float vel_camino;
   float vel_edificios;

	//SDL_Surface *surfaceTexto;
	//SDL_Texture *texturaTexto;
	SDL_Color color1;
	SDL_Color color2;
	SDL_Color color3;
} menu_principal_recursos;

/* ----------------------------------------------------------------
   input.c actualiza a través de la variable GameState, update.c lo lee y reacciona.
   ---------------------------------------------------------------- */
typedef struct {
    bool keyUp;
    bool keyDown;
    // Puedes agregar más teclas o combinaciones de teclas
} InputState;



SDL_Window *crear_ventana();
/* ----------------------------------------------------------------
   game.c
   ---------------------------------------------------------------- */

typedef struct {
	SDL_Window *ventana;
	SDL_Renderer *renderizado;
	bool corriendo;
	int last_frame_time;
	ESTADO_ACTUAL estado_juego;

	entidad_rec_simple base;
	entidad_rec_simple cae;
	menu_principal_recursos rec_menu;
	float iris_radius;
   bool is_iris_fading_out;
} eventos_globales;

typedef struct {
	bool single_player;
	bool niveles_aleatorios;
	int dificultad;
} tipo_partida;
/* ----------------------------------------------------------------
   assets.c
   ---------------------------------------------------------------- */
void assets_load(void);

/* COMMONS_H */
void iniciar_componente(bool test, const char *componente);
void Pantalla_Completa(SDL_Window *ventana);

ESTADO_ACTUAL menu_principal(eventos_globales *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu);
ESTADO_ACTUAL juego_principal(eventos_globales *ev_gl, SDL_Event *evento);
void generar_rectangulo(entidad_rec_simple *rectangulo);
void update(float delta_time, eventos_globales *ev_gl);
ESTADO_ACTUAL seleccionar_niveles(void);
#endif
