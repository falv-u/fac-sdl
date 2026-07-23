#include "commons.h"

/* funciones utiles */

void
Pantalla_Completa(SDL_Window *ventana)
{
    Uint32 flags_actuales = SDL_GetWindowFlags(ventana);

    if (flags_actuales & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        SDL_SetWindowFullscreen(ventana, 0);
    } else {
        SDL_SetWindowFullscreen(ventana, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
}

