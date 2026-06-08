/* Librerias estandar */
#include <stdio.h>

/* librerias sistema no estandar */
/* headers propios */
#include "commons.h"
#include "log.h"

void end( SDL_Window *ventana);

int main(int argc, char** argv)
{
	
	ESTADO_ACTUAl estado_juego = ESTADO_MENU;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		game_log(LOG_ERROR, "SDL no pudo inicializarse! Error: %s\n", SDL_GetError());
	} else {

		game_log(LOG_INFO, "¡Componentes de SDL iniciados correctamente!\n", 0);
	}

	bool corriendo = true;

	crear_ventana();
   	while (corriendo)
	{
	
      SDL_Event evento;
        
      while (SDL_PollEvent(&evento))
		{
			if (evento.type == SDL_QUIT) corriendo = false;
				
			switch (estado_juego)
			{
				case ESTADO_MENU:
					break;
				case ESTADO_CARGA:
					break;
				case ESTADO_PAUSA:
					break;
				case ESTADO_JUEGO:
					break;
				case ESTADO_GAMEOVER:
					break;
				case ESTADO_SALIR:
					break;
				default:
					break;
			}

		}

	}
	return 0;
}

void end( SDL_Window *ventana) 
{

	SDL_Quit();
}
