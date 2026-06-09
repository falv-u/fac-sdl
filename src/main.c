/* Librerias estandar */
#include <stdbool.h>
#include <stdio.h>

/* librerias sistema no estandar */
/* headers propios */
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "commons.h"
#include "log.h"

void end(SDL_Window *ventana);
void eventos_globales_accionados_simples(eventos_globales *ev_gl, SDL_Event evento);
int main(int argc, char** argv)
{
	eventos_globales ev_gl;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		game_log(LOG_ERROR, "SDL no pudo inicializarse! Error: %s\n", SDL_GetError());
	} else {

		game_log(LOG_DEBUG, "Componentes de SDL iniciados correctamente!\n", 0);
	}

	ESTADO_ACTUAl estado_juego = ESTADO_MENU;
	
	ev_gl.corriendo = true;
	ev_gl.ventana = crear_ventana();
	ev_gl.renderizado = SDL_CreateRenderer(ev_gl.ventana, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(ev_gl.renderizado, ANCHO_PANTALLA, LARGO_PANTALLA);
   while (ev_gl.corriendo)
	{
      SDL_Event evento;
        
      while (SDL_PollEvent(&evento))
		{
			eventos_globales_accionados_simples(&ev_gl, evento);
			if (evento.key.keysym.sym == SDLK_F9 && evento.key.repeat == 0)
				Pantalla_Completa(ev_gl.ventana);

			switch (estado_juego)
				{
					case ESTADO_MENU:
						estado_juego=menu_principal(ev_gl, &evento, estado_juego);
						break;
					case ESTADO_CARGA:
						break;
					case ESTADO_PAUSA:
						break;
					case ESTADO_JUEGO:
						estado_juego=juego_principal(ev_gl, &evento, estado_juego);
						break;
					case ESTADO_GAMEOVER:
						break;
					case ESTADO_SALIR:
						ev_gl.corriendo = false;
						game_log(LOG_DEBUG, "ESTADO_SALIR HA SIDO RETORNADO POR UNA FUNCION", 0);
						break;
					default:
						break;
				}

		}

	}
	end(ev_gl.ventana);
	return 0;
}

void end( SDL_Window *ventana) 
{
	SDL_DestroyWindow(ventana);
	SDL_Quit();
}


void eventos_globales_accionados_simples(eventos_globales *ev_gl, SDL_Event evento)
{
	if (evento.type == SDL_QUIT) 
	{
			game_log(LOG_INFO, "Cerrando ventana por accion del usuario a traves del servidor grafico ", 0);
			ev_gl->corriendo = false;
	}
	if (evento.type == SDL_KEYDOWN &&  evento.key.repeat == 0)
	{
		if (evento.key.keysym.sym == SDLK_F9)
			Pantalla_Completa(ev_gl->ventana);
		if (evento.key.keysym.sym == SDLK_ESCAPE)
		{
			game_log(LOG_DEBUG, "cierre por fin del loop a traves de la bandera 'corriendo', accionado por tecla ESCAPE", 0);
			ev_gl->corriendo = false;
		}
	}

}
