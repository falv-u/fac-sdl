#include "SDL_events.h"
#include "SDL_rect.h"
#include "SDL_render.h"

#include "commons.h"
#include <math.h>

void renderizar_fractales(eventos_globales ev_gl);
void fondo_menu(eventos_globales *ev_gl, menu_principal_recursos *rec_menu);
void fade_iris_out(eventos_globales *ev_gl);

void pintar_gradiente_noche_menu(SDL_Renderer *renderizado);

ESTADO_ACTUAL menu_principal(eventos_globales *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu)
{
	(void)evento; /* para eliminar warning, no relevante */

	SDL_RenderClear(ev_gl->renderizado);
	
	pintar_gradiente_noche_menu(ev_gl->renderizado);
	fondo_menu(ev_gl, rec_menu);


	if (ev_gl->is_iris_fading_out)
		fade_iris_out(ev_gl);

 	/* RECORDAR DESTRUIR TEXTURAS */
	SDL_RenderPresent(ev_gl->renderizado);
	return ESTADO_MENU;
}

void fondo_menu(eventos_globales *ev_gl, menu_principal_recursos *rec_menu)
{

	int ancho_final=64*8*ESCALADO_1;
	int alto_final=64*ESCALADO_1;


	if (rec_menu->sprites) 
	{
		SDL_Rect src_moon = {14*64, 0*64, 64*2, 64*2};
		SDL_Rect dest_moon = {100, 100, 64*2, 64*2};
		SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_moon, &dest_moon);

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
void fade_iris_out(eventos_globales *ev_gl)
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

void pintar_gradiente_noche_menu(SDL_Renderer *render)
{
	SDL_Color color_cenit = {5, 10, 35, 255};   /* Azul noche superior */
	SDL_Color color_horizonte = {0, 0, 5, 255}; /* horizonte inferior */

	SDL_Vertex vertices[4];

	/* Arriba Izquierda */
	vertices[0].position.x = 0.0f;
	vertices[0].position.y = 0.0f;
	vertices[0].color = color_cenit;
	vertices[0].tex_coord.x = 0.0f;
	vertices[0].tex_coord.y = 0.0f;

	/* Arriba Derecha */
	vertices[1].position.x = (float)ANCHO_PANTALLA;
	vertices[1].position.y = 0.0f;
	vertices[1].color = color_cenit;
	vertices[1].tex_coord.x = 1.0f;
	vertices[1].tex_coord.y = 0.0f;

	/* Abajo Derecha */
	vertices[2].position.x = (float)ANCHO_PANTALLA;
	vertices[2].position.y = (float)LARGO_PANTALLA;
	vertices[2].color = color_horizonte;
	vertices[2].tex_coord.x = 1.0f;
	vertices[2].tex_coord.y = 1.0f;

	/* Abajo Izquierda */
	vertices[3].position.x = 0.0f;
	vertices[3].position.y = (float)LARGO_PANTALLA;
	vertices[3].color = color_horizonte;
	vertices[3].tex_coord.x = 0.0f;
	vertices[3].tex_coord.y = 1.0f;

	int indices[6] = {0, 1, 2, 0, 2, 3};

	/* Renderizado de geometría sin textura (NULL) para usar solo color */
	SDL_RenderGeometry(render, NULL, vertices, 4, indices, 6);
}
