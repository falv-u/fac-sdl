#include "SDL_events.h"
#include "SDL_image.h"

#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include "commons.h"

void renderizar_fractales(eventos_globales ev_gl);
void fondo_menu(eventos_globales *ev_gl, menu_principal_recursos *rec_menu);
ESTADO_ACTUAL menu_principal(eventos_globales *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu)
{
	(void)evento;
	SDL_RenderClear(ev_gl->renderizado);

	fondo_menu(ev_gl, rec_menu);


	if (ev_gl->is_iris_fading_out)
	{
		SDL_SetRenderDrawColor(ev_gl->renderizado, 0, 0, 0, 255);

		int w = ANCHO_PANTALLA;
		int h = LARGO_PANTALLA;
		float r = ev_gl->iris_radius;
		int cx = w / 2;
		int cy = h / 2;

		SDL_Rect top    = {0, 0, w, cy - (int)r};
		SDL_Rect bottom = {0, cy + (int)r, w, h - (cy + (int)r)};
		SDL_Rect left   = {0, cy - (int)r, cx - (int)r, (int)r * 2};
		SDL_Rect right  = {cx + (int)r, cy - (int)r, w - (cx + (int)r), (int)r * 2};

		SDL_RenderFillRect(ev_gl->renderizado, &top);
		SDL_RenderFillRect(ev_gl->renderizado, &bottom);
		SDL_RenderFillRect(ev_gl->renderizado, &left);
		SDL_RenderFillRect(ev_gl->renderizado, &right);
	}

 	/* RECORDAR DESTRUIR TEXTURAS */

	SDL_RenderPresent(ev_gl->renderizado);
	return ESTADO_MENU;
}

void fondo_menu(eventos_globales *ev_gl, menu_principal_recursos *rec_menu)
{
	/*
	SDL_RenderPresent(ev_gl->renderizado);
	if (evento != NULL && evento->type == SDL_KEYDOWN)
	{
		if (evento->key.keysym.sym == SDLK_ESCAPE) 
			return ESTADO_SALIR;

		if (evento->key.keysym.sym == SDLK_RETURN) 
			return ESTADO_JUEGO;
	}
	*/
	SDL_Rect src_farol = { 3 * 64, 1 * 64, 64, 64 };
	SDL_Rect dest_farol = { 300, 450, 64 * 2, 64 * 2 };

	if (rec_menu->sprites) 
		SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_farol, &dest_farol);
	

	SDL_Rect rectDestino = { 100, 100, rec_menu->titulo_w, rec_menu->titulo_h };
	if (rec_menu->textura_titulo) 
		SDL_RenderCopy(ev_gl->renderizado, rec_menu->textura_titulo, NULL, &rectDestino);
	
}

