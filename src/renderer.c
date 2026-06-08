
#include "commons.h"

void crear_ventana()
{
	SDL_Window *ventana = SDL_CreateWindow(
        "feel, amplify and conquer en SDL2", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        ANCHO_PANTALLA, 
        LARGO_PANTALLA, 
        SDL_WINDOW_SHOWN
    );

	
	SDL_Delay(3000); 

	SDL_DestroyWindow(ventana);
}


