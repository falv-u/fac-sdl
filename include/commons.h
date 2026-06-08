#ifndef COMMONS_H
#define COMMONS_H

#include <stdbool.h>


/* SDL libs */
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "SDL_video.h"

/* Macros */
#define TARGET_FPS      60

#define LARGO_PANTALLA 400
#define ANCHO_PANTALLA 600

/* Maquina de estado */

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


/* ----------------------------------------------------------------
   Estado del juego
   La estructura central del proyecto. Representa todo lo que
   está pasando en el juego en un momento dado.
   Casi todos los módulos la leen. Solo update.c la modifica.
   ---------------------------------------------------------------- */

typedef struct {
    ESTADO_ACTUAl screen;      /* pantalla activa */
    InputState input;    /* estado del input */  
    int level; /*nivel actual*/

    bool running; /* mantiene el juego corriendo */
    /* Agrega los campos que necesite tu juego */
} GameState;

/* ================================================================
   PROTOTIPOS (de funciones). 
   Concentra los prototipos de todos los archivos .c
   ================================================================ */


/* ----------------------------------------------------------------
   game.c
   ---------------------------------------------------------------- */
void game_init(GameState *gs);


/* ----------------------------------------------------------------
   input.c
   ---------------------------------------------------------------- */
void input_update(InputState *input);


/* ----------------------------------------------------------------
   update.c
   ---------------------------------------------------------------- */
void update(GameState *gs, InputState *input);


/*   renderer.c */
void crear_ventana();

/* ----------------------------------------------------------------
   ui.c
   ---------------------------------------------------------------- */
void render_ui(GameState *gs);


/* ----------------------------------------------------------------
   assets.c
   ---------------------------------------------------------------- */
void assets_load(void);

/* COMMONS_H */
void iniciar_componente(bool test, const char *componente);

/* ----------------------------------------------------------------
   entities.c
   ---------------------------------------------------------------- */
/* void enemy_update(GameState *gs, int enemy_index);            */
/* void player_jump(GameState *gs);                              */
/* bool check_collision(float x1, float y1, float x2, float y2); */

#endif
