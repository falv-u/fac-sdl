#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_keycode.h"
#include "commons.h"
void renderizar_fractales(eventos_globales ev_gl);
ESTADO_ACTUAL menu_principal(eventos_globales *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu)
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


	SDL_RenderPresent(ev_gl->renderizado);
	SDL_RenderClear(ev_gl->renderizado);

	SDL_Surface *imagen = IMG_Load("/home/frani/code/fac-sdl/assets/fondos/puf.png");
	SDL_Texture* textura_fondo = SDL_CreateTextureFromSurface(ev_gl->renderizado, imagen);
	SDL_FreeSurface(imagen); 

	SDL_RenderCopy(ev_gl->renderizado, textura_fondo, NULL, NULL);
	SDL_DestroyTexture(textura_fondo); // Destrucción obligatoria que faltaba

	SDL_Surface *surfaceTexto = TTF_RenderText_Solid(rec_menu->fuente, "Feel, Amplify and Conquer!", rec_menu->color2);
	SDL_Texture *texturaTexto = SDL_CreateTextureFromSurface(ev_gl->renderizado, surfaceTexto);
	SDL_Rect rectDestino = {100, 100, surfaceTexto->w, surfaceTexto->h };
	SDL_FreeSurface(surfaceTexto);

	SDL_RenderCopy(ev_gl->renderizado, texturaTexto, NULL, &rectDestino);
	SDL_DestroyTexture(texturaTexto);


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

	SDL_RenderPresent(ev_gl->renderizado);
	return ESTADO_MENU;
}


void renderizar_fractales(eventos_globales ev_gl)
{

}

