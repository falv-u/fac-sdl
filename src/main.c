/* Librerias estandar */
#include <stdbool.h>

/* librerias sistema no estandar */
/* headers propios */
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_ttf.h"
#include "commons.h"
#include "log.h"

void end(SDL_Window *ventana);
void eventos_globales_accionados_simples(eventos_globales *ev_gl, SDL_Event evento);
int main(int argc, char** argv)
{
	eventos_globales ev_gl;
	entidad_rec_simple rec;
	menu_principal_recursos rec_menu;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | TTF_Init() ) < 0)
	{
		game_log(LOG_ERROR, "SDL no pudo inicializarse! Error: %s\n", SDL_GetError());
	} else {

		game_log(LOG_DEBUG, "Componentes de SDL iniciados correctamente!\n", 0);
	}
	ESTADO_ACTUAL estado_juego = ESTADO_MENU;
	
	ev_gl.corriendo = true;
	ev_gl.ventana = crear_ventana();
	ev_gl.renderizado = SDL_CreateRenderer(ev_gl.ventana, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(ev_gl.renderizado, ANCHO_PANTALLA, LARGO_PANTALLA);
	
	rec.rectangulo.x = 250;
	rec.rectangulo.y = 250;
	rec.rectangulo.h = 100;
	rec.rectangulo.w = 100;
	/* Recursos del menu */
	rec_menu.fuente = TTF_OpenFont("/home/frani/code/fac-sdl/assets/fonts/Silver.ttf", 64);

	rec_menu.color1.r = 255;
	rec_menu.color1.g = 255;
	rec_menu.color1.g = 255;

	rec_menu.color2.r = 70;
	rec_menu.color2.g = 255;
	rec_menu.color2.b = 0;

	rec_menu.color3.r = 0;
	rec_menu.color3.g = 34;
	rec_menu.color3.b = 255;
	
   while (ev_gl.corriendo)
	{
      SDL_Event evento;
        
      while (SDL_PollEvent(&evento))
		{
			eventos_globales_accionados_simples(&ev_gl, evento);

			switch (estado_juego)
				{
					case ESTADO_MENU:
						estado_juego=menu_principal(ev_gl, &evento, estado_juego, &rec_menu);
						break;
					case ESTADO_CARGA:
						break;
					case ESTADO_PAUSA:
						break;
					case ESTADO_JUEGO:
						estado_juego=juego_principal(ev_gl, &evento, estado_juego, &rec);
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
