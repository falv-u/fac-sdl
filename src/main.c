/* Librerias estandar */
#include <stdbool.h>
#include <stdio.h>

/* librerias sistema no estandar */
/* headers propios */
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "commons.h"
#include "log.h"

void end(SDL_Window *ventana);
void eventos_globales_accionados_simples(eventos_globales *ev_gl, SDL_Event evento);



int main(int argc, char** argv)
{
	(void)(argc);
	(void)(argv);
	eventos_globales ev_gl;
	menu_principal_recursos rec_menu;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | TTF_Init() ) < 0)
	{
		game_log(LOG_ERROR, "SDL no pudo inicializarse! Error: %s\n", SDL_GetError());
	} else {

		game_log(LOG_DEBUG, "Componentes de SDL iniciados correctamente!\n", 0);
	}

   IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	ev_gl.estado_juego=ESTADO_MENU;
	ev_gl.corriendo = true;
	ev_gl.ventana = crear_ventana();
	ev_gl.renderizado = SDL_CreateRenderer(ev_gl.ventana, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(ev_gl.renderizado, ANCHO_PANTALLA, LARGO_PANTALLA);
	
	ev_gl.base.rectangulo.x = 200;
	ev_gl.base.rectangulo.y = 600;
	ev_gl.base.rectangulo.h = 50;
	ev_gl.base.rectangulo.w = 100;
	
	ev_gl.cae.rectangulo.x = 200;
	ev_gl.cae.rectangulo.y = 10;
	ev_gl.cae.rectangulo.h = 100;
	ev_gl.cae.rectangulo.w = 50;
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
	
	unsigned int last_frame_time = SDL_GetTicks();

	while (ev_gl.corriendo)
	{
      SDL_Event evento;
    	/* DELTA TIME */
		unsigned int current_time = SDL_GetTicks();
		float delta_time = (current_time - last_frame_time) / 1000.0f;
		last_frame_time = current_time;	
		update(delta_time, &ev_gl);
		unsigned int frame_duration = SDL_GetTicks() - current_time;
		if (frame_duration < FRAME_TARGET_TIME)
			SDL_Delay(FRAME_TARGET_TIME - frame_duration);

      while (SDL_PollEvent(&evento))
		{
			eventos_globales_accionados_simples(&ev_gl, evento);
			if (evento.type == SDL_KEYDOWN && evento.key.repeat == 0)
			{
				if (evento.key.keysym.sym == SDLK_RETURN)
				{
					if (ev_gl.estado_juego == ESTADO_MENU) ev_gl.estado_juego = ESTADO_JUEGO;
				}
			
			}
		}
		switch (ev_gl.estado_juego)
			{
				case ESTADO_MENU:
					ev_gl.estado_juego=menu_principal(&ev_gl, &evento, &rec_menu);
					break;
				case ESTADO_CARGA:
					break;
				case ESTADO_PAUSA:
					break;
				case ESTADO_JUEGO:
					ev_gl.estado_juego=juego_principal(&ev_gl, &evento);
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
