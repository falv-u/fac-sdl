/* Librerias estandar */
#include <stdbool.h>

/* librerias sistema no estandar */
/* headers propios */
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "commons.h"
#include "log.h"

void iniciar_componente();
void iniciar_recursos_menu(menu_principal_recursos *rec_menu, eventos_globales *ev_gl);
void iniciar_eventos_globales(eventos_globales *ev_gl);
void manejo_delta_time(float *delta_time, unsigned int *last_frame_time);

void eventos_globales_accionados_simples(eventos_globales *ev_gl, SDL_Event evento);
void eventos_accionados_usuario(eventos_globales *ev_gl, SDL_Event evento, menu_principal_recursos *rec_menu);
void teclas_menu_principal(menu_principal_recursos *rec_menu, SDL_Event evento);

int main(void)
{
	eventos_globales ev_gl;
	menu_principal_recursos rec_menu;
	
	float delta_time=0;
	unsigned int last_frame_time = SDL_GetTicks();

	iniciar_componente();
	iniciar_eventos_globales(&ev_gl); /* la ventana tambien se crea aqui al estar anclada a los eventos globales como cerrar */
	iniciar_recursos_menu(&rec_menu, &ev_gl);

	while (ev_gl.corriendo)
	{
      SDL_Event evento;

		manejo_delta_time(&delta_time, &last_frame_time);
		update(delta_time, &ev_gl,  &rec_menu);

		/* ignorar warning ya que se declara al incio de la funcion para los eventos de usuario */ 
		eventos_accionados_usuario(&ev_gl, evento, &rec_menu); 
      
		switch (ev_gl.estado_juego)
			{
				case ESTADO_MENU:
					ev_gl.estado_juego=menu_principal(&ev_gl, &evento, &rec_menu);
					break;
				case ESTADO_CARGA:
					break;
				case ESTADO_PAUSA:
					break;
				case ESTADO_SELECT_TIPO_PARTIDA:
					break;
				case ESTADO_SELECT_NIVELES:
					ev_gl.estado_juego=seleccionar_niveles();
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

	SDL_DestroyWindow(ev_gl.ventana);
	SDL_Quit();
	return 0;
}


void iniciar_recursos_menu(menu_principal_recursos *rec_menu, eventos_globales *ev_gl)
{
	rec_menu->opcion=1;
	rec_menu->fuente = TTF_OpenFont("./assets/fonts/Silver.ttf", 64);
	rec_menu->sprites = IMG_LoadTexture(ev_gl->renderizado, "./assets/Sprite-0002.png");

	rec_menu->color1.r = 255;
	rec_menu->color1.g = 255;
	rec_menu->color1.b = 255;

	rec_menu->color2.r = 70;
	rec_menu->color2.g = 255;
	rec_menu->color2.b = 0;

	rec_menu->color3.r = 0;
	rec_menu->color3.g = 34;
	rec_menu->color3.b = 255;
	
	
	
	if (rec_menu->fuente) 
	{
        SDL_Surface *surf = TTF_RenderText_Solid(rec_menu->fuente, "Feel, Amplify and Conquer!", rec_menu->color2);
        rec_menu->textura_titulo = SDL_CreateTextureFromSurface(ev_gl->renderizado, surf);
        rec_menu->titulo_w = surf->w;
        rec_menu->titulo_h = surf->h;
        SDL_FreeSurface(surf);
   }

	rec_menu->scroll_edificios = 0.0f;
   rec_menu->vel_edificios = 300.0f;

}

void iniciar_eventos_globales(eventos_globales *ev_gl)
{

	ev_gl->estado_juego=ESTADO_MENU;
	ev_gl->corriendo = true;
	ev_gl->ventana = crear_ventana();
	ev_gl->renderizado = SDL_CreateRenderer(ev_gl->ventana, -1, SDL_RENDERER_ACCELERATED);
	ev_gl->iris_radius = 800.0;
	ev_gl->is_iris_fading_out = false;
	SDL_RenderSetLogicalSize(ev_gl->renderizado, ANCHO_PANTALLA, LARGO_PANTALLA);

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

void iniciar_componente()
{

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
		game_log(LOG_ERROR, "SDL no pudo inicializarse! Error: %s\n", SDL_GetError());
	
	if (TTF_Init() == -1) 
		game_log(LOG_ERROR, "SDL_ttf error: %s\n", TTF_GetError());

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	game_log(LOG_DEBUG, "Componentes de SDL iniciados correctamente!\n", 0);
}

void manejo_delta_time(float *delta_time, unsigned int *last_frame_time)
{
	unsigned int current_time = SDL_GetTicks();

	*delta_time = (current_time - *last_frame_time) / 1000.0f;

	*last_frame_time = SDL_GetTicks();

	if (*delta_time > 0.1f) *delta_time = 0.1f;

	*last_frame_time = current_time;	

	unsigned int frame_duration = SDL_GetTicks() - current_time;
	if (frame_duration < FRAME_TARGET_TIME)
		SDL_Delay(FRAME_TARGET_TIME - frame_duration);
}

void eventos_accionados_usuario(eventos_globales *ev_gl, SDL_Event evento, menu_principal_recursos *rec_menu)
{
	while (SDL_PollEvent(&evento))
		{
			if (evento.type == SDL_KEYDOWN && evento.key.repeat == 0)
			{
				eventos_globales_accionados_simples(ev_gl, evento);
				teclas_menu_principal(rec_menu, evento);
			}
		}

}

void teclas_menu_principal(menu_principal_recursos *rec_menu, SDL_Event evento)
{
	if (evento.key.keysym.sym == SDLK_UP && rec_menu->opcion > 0)
		rec_menu->opcion--;
	if (evento.key.keysym.sym == SDLK_DOWN && rec_menu->opcion < 3)
		rec_menu->opcion++;
	if (rec_menu->opcion > 3) 
		rec_menu->opcion = 0;
	if (rec_menu->opcion < 0) 
		rec_menu->opcion=3;
	
	
}
