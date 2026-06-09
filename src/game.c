#include "commons.h"


ESTADO_ACTUAL juego_principal(eventos_globales ev_gl, SDL_Event *evento, ESTADO_ACTUAL estado_actual, entidad_rec_simple *rec )
{

	if (evento->type == SDL_KEYDOWN )
	{
		if (evento->key.keysym.sym == SDLK_UP)
			rec->rectangulo.y -= 10; 
		if (evento->key.keysym.sym == SDLK_DOWN)
			rec->rectangulo.y += 10; 
		if (evento->key.keysym.sym == SDLK_RIGHT)
			rec->rectangulo.x += 10; 
		if (evento->key.keysym.sym == SDLK_LEFT)
			rec->rectangulo.x -= 10; 
	}
	SDL_SetRenderDrawColor(ev_gl.renderizado, 0, 0, 0, 255); // Negro
	SDL_RenderClear(ev_gl.renderizado);
	
	SDL_SetRenderDrawColor(ev_gl.renderizado, 255, 0, 0, 255); // Rojo

	SDL_RenderFillRect(ev_gl.renderizado, &rec->rectangulo);

	SDL_RenderPresent(ev_gl.renderizado);
	SDL_Delay(9); 
	return estado_actual;
}



