#include "commons.h"


ESTADO_ACTUAL juego_principal(eventos_globales *ev_gl, SDL_Event *evento)
{
	

	SDL_SetRenderDrawColor(ev_gl->renderizado, 0, 0, 0, 255); // Negro
	SDL_RenderClear(ev_gl->renderizado);
	
	SDL_SetRenderDrawColor(ev_gl->renderizado, 255, 0, 0, 255); 
   SDL_RenderFillRect(ev_gl->renderizado, &ev_gl->base.rectangulo);

    // 3. Dibujar la entidad 'cae' (Verde, para diferenciarla de la base)
   SDL_SetRenderDrawColor(ev_gl->renderizado, 0, 255, 0, 255); 
   SDL_RenderFillRect(ev_gl->renderizado, &ev_gl->cae.rectangulo);
	SDL_RenderPresent(ev_gl->renderizado);

	return ESTADO_JUEGO;
}



