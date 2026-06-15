#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_keycode.h"
#include "commons.h"
void renderizar_fractales(eventos_globales ev_gl);
ESTADO_ACTUAL menu_principal(eventos_globales *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu)
{
	/*
	if (evento != NULL && evento->type == SDL_KEYDOWN)
	{
		if (evento->key.keysym.sym == SDLK_ESCAPE) 
			return ESTADO_SALIR;

		if (evento->key.keysym.sym == SDLK_RETURN) 
			return ESTADO_JUEGO;
	}
	*/

	SDL_RenderClear(ev_gl->renderizado);

	SDL_Surface *imagen = IMG_Load("/home/frani/code/fac-sdl/assets/puf.png");
	SDL_Texture* textura_fondo = SDL_CreateTextureFromSurface(ev_gl->renderizado, imagen);
	SDL_FreeSurface(imagen); 

	SDL_RenderCopy(ev_gl->renderizado, textura_fondo, NULL, NULL);
	SDL_DestroyTexture(textura_fondo); // Destrucción obligatoria que faltaba

	SDL_Surface *surfaceTexto = TTF_RenderText_Solid(rec_menu->fuente, "Feel, Amplify and Conquer!", rec_menu->color1);
	SDL_Texture *texturaTexto = SDL_CreateTextureFromSurface(ev_gl->renderizado, surfaceTexto);
	SDL_Rect rectDestino = {100, 100, surfaceTexto->w, surfaceTexto->h };
	SDL_FreeSurface(surfaceTexto);

	SDL_RenderCopy(ev_gl->renderizado, texturaTexto, NULL, &rectDestino);
	SDL_DestroyTexture(texturaTexto);

	SDL_RenderPresent(ev_gl->renderizado);

	return ESTADO_MENU;
}


void renderizar_fractales(eventos_globales ev_gl)
{

}

