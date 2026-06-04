#ifndef COMMONS_H
#define COMMONS_H

/* ================================================================
   COMMONS.H — Header central del proyecto
   Todos los .c incluyen este archivo y solo este.
   Agrega aquí solo lo que sea necesario en múltiples módulos.
   ================================================================ */

/* ================================================================
   INCLUDES (librerias)
   ================================================================ */

#include <stdio.h>
#include <stdbool.h>

/* ================================================================
   DEFINE (constantes)
   ================================================================ */

#define TARGET_FPS      60

/* ================================================================
   ENUMS (manejo de estados)
   ================================================================ */

typedef enum {
    SCREEN_MENU,
    SCREEN_PLAYING,
    SCREEN_GAMEOVER
} Screen;

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
    Screen screen;      /* pantalla activa */
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


/* ----------------------------------------------------------------
   renderer.c
   ---------------------------------------------------------------- */
void render_gameview(GameState *gs);


/* ----------------------------------------------------------------
   ui.c
   ---------------------------------------------------------------- */
void render_ui(GameState *gs);


/* ----------------------------------------------------------------
   assets.c
   ---------------------------------------------------------------- */
void assets_load(void);

/* ----------------------------------------------------------------
   entities.c
   ---------------------------------------------------------------- */
/* void enemy_update(GameState *gs, int enemy_index);            */
/* void player_jump(GameState *gs);                              */
/* bool check_collision(float x1, float y1, float x2, float y2); */

#endif
