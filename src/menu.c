#include "SDL_events.h"
#include "SDL_image.h"

#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include "commons.h"

void renderizar_fractales(eventos_globales ev_gl);
void fondo_menu(eventos_globales *ev_gl, menu_principal_recursos *rec_menu);
void update_notas(float dt, MapaCancion *mapa, float velocidad_caida);
void procesar_input_carril(MapaCancion *mapa, int carril_presionado, SDL_Rect *hitbox_juez);

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

	if (rec_menu->sprites) 
	{
		int ancho_final=64*8*ESCALADO_1;
		int alto_final=64*ESCALADO_1;

		SDL_Rect src_arboleda = { 4*64, 0*64, (64*8), 64 };
		SDL_Rect dest_arboleda_0;
		dest_arboleda_0.x = (int)rec_menu->scroll_edificios; 
		dest_arboleda_0.y = 470;
		dest_arboleda_0.w = ancho_final;
		dest_arboleda_0.h = alto_final;

		SDL_Rect dest_arboleda_1;
		dest_arboleda_1.x = dest_arboleda_0.x+ancho_final;
		dest_arboleda_1.y = dest_arboleda_0.y;
		dest_arboleda_1.w = dest_arboleda_0.w;
		dest_arboleda_1.h = dest_arboleda_0.h;

		SDL_Rect dest_arboleda_2;
      dest_arboleda_2.x = dest_arboleda_1.x + ancho_final; 
      dest_arboleda_2.y = dest_arboleda_0.y;
      dest_arboleda_2.w = ancho_final;
      dest_arboleda_2.h = alto_final;

		SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_arboleda, &dest_arboleda_0);
      SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_arboleda, &dest_arboleda_1);
      SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_arboleda, &dest_arboleda_2);
	}

	SDL_Rect rectDestino = { 100, 100, rec_menu->titulo_w, rec_menu->titulo_h };
	if (rec_menu->textura_titulo) 
		SDL_RenderCopy(ev_gl->renderizado, rec_menu->textura_titulo, NULL, &rectDestino);
	
}

void update_notas(float dt, MapaCancion *mapa, float velocidad_caida)
{
	for (int i = 0; i < mapa->total_notas; i++)
	{
		Nota *n = &mapa->notas[i];

		if (n->estado == NOTA_ESPERANDO || n->estado == NOTA_MANTENIDA)
		{
			n->hitbox.y += (int)(velocidad_caida * dt);
		}

		if (n->hitbox.y > LARGO_PANTALLA && n->estado != NOTA_GOLPEADA)
		{
			n->estado = NOTA_FALLADA;
		}
	}
}

void procesar_input_carril(MapaCancion *mapa, int carril_presionado, SDL_Rect *hitbox_juez)
{
	for (int i = 0; i < mapa->total_notas; i++)
	{
		Nota *n = &mapa->notas[i];

		if (n->carril == carril_presionado && n->estado == NOTA_ESPERANDO)
		{
			if (SDL_HasIntersection(&n->hitbox, hitbox_juez))
			{
				if (n->tipo == NOTA_SIMPLE) 
				{
					n->estado = NOTA_GOLPEADA; 
				} else if (n->tipo == NOTA_LARGA) {
					n->estado = NOTA_MANTENIDA; 
				}
				break; 
			}
		}
	}
}
