#include "commons.h"


ESTADO_ACTUAL juego_principal(eventos_globales *ev_gl, SDL_Event *evento)
{

	(void)evento; /* evita warning, ignora */

   SDL_SetRenderDrawColor(ev_gl->renderizado, 15, 15, 20, 255);
	SDL_RenderClear(ev_gl->renderizado);

	/* Calculamos el margen izquierdo para centrar 8 carriles de 64px */
	int x_base = (int)((ANCHO_PANTALLA / 2) - ((8 * 64 + 64) / 2));

	/* dibujar linae juez */
	SDL_SetRenderDrawColor(ev_gl->renderizado, 100, 100, 100, 255); 
	for (int carril = 0; carril < 8; carril++) 
	{
		int x_carril_juez = x_base + (carril * 64);
		if (carril >= 4) x_carril_juez += 64; /* Espacio de separación P1 y P2 */

		SDL_Rect rect_juez = { x_carril_juez, (int)Y_JUEZ, 64, 32 };
		SDL_RenderDrawRect(ev_gl->renderizado, &rect_juez);
	}

	/* dibujar notas activas */
	for (int i = ev_gl->mapa_actual.notas_pasadas; i < ev_gl->mapa_actual.total_notas; i++) 
	{
		Nota *n = &ev_gl->mapa_actual.arreglo_notas[i];

		if (n->activa == false || n->y_actual <= Y_SPAWN)
			continue;

		/* Determinamos el color (Cyan para P1, Magenta para P2) */
		if (n->carril < 4) 
			SDL_SetRenderDrawColor(ev_gl->renderizado, 0, 255, 255, 255); 
		else 
			SDL_SetRenderDrawColor(ev_gl->renderizado, 255, 0, 255, 255); 

		/* Matematica de la primitiva grafica */
		float velocidad = (Y_JUEZ - Y_SPAWN) / TIEMPO_APROXIMACION;

		/* Altura base para nota normal (32px), si es larga, escalamos por el tiempo */
		int alto_primitiva = (n->duracion > 0.0f) ? (int)(velocidad * n->duracion) : 32;

		/* El ancho dinamico considerando que sera de dos players */
		int x_carril = x_base + (n->carril * 64);

		if (n->carril >= 4)
			x_carril += 64; 

		SDL_Rect rect_nota = {
		x_carril,
		(int)(n->y_actual - alto_primitiva), 
		64,
		alto_primitiva
		};

		SDL_RenderFillRect(ev_gl->renderizado, &rect_nota);
	}

	SDL_RenderPresent(ev_gl->renderizado);
	return ESTADO_JUEGO;
}



