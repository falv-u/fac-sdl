#ifndef COMMONS_H
#define COMMONS_H

#include <stdbool.h>

#include "SDL_events.h"
#include "SDL_render.h"
#include "log.h"

/* SDL libs */
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "SDL_video.h"

/* Macros */
#define TARGET_FPS      60

#define LARGO_PANTALLA 400
#define ANCHO_PANTALLA 600 
#define LOG_LEVEL LOG_DEBUG /* Maquina de estado */
typedef enum {
    ESTADO_MENU,
	 ESTADO_PAUSA,
	 ESTADO_CARGA,
    ESTADO_JUEGO,
    ESTADO_GAMEOVER,
	 ESTADO_SALIR,
} ESTADO_ACTUAl;

/* ================================================================
   ESTRUCTURAS
   ================================================================ */

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
} eventos_globales;

/* ----------------------------------------------------------------
   assets.c
   ---------------------------------------------------------------- */
void assets_load(void);

/* COMMONS_H */
void iniciar_componente(bool test, const char *componente);
void Pantalla_Completa(SDL_Window *ventana);
/* ----------------------------------------------------------------
   entities.c
   ---------------------------------------------------------------- */
/* void enemy_update(GameState *gs, int enemy_index);            */
/* void player_jump(GameState *gs);                              */
/* bool check_collision(float x1, float y1, float x2, float y2); */

ESTADO_ACTUAl menu_principal(eventos_globales, SDL_Event *evento, ESTADO_ACTUAl estado_actual);
ESTADO_ACTUAl juego_principal(eventos_globales, SDL_Event *evento, ESTADO_ACTUAl estado_actual);
#endif
