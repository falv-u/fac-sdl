#include "commons.h"
#include <SDL_image.h>

void update(float dt, eventos_globales *ev_gl, menu_principal_recursos *rec_menu)
{
	if (ev_gl->is_iris_fading_out == true)
	{
		ev_gl->iris_radius -= 800.0*dt;
	}

	switch (ev_gl->estado_juego)
	{
		case ESTADO_MENU:
			rec_menu->scroll_edificios -= rec_menu->vel_edificios * dt;

			if (rec_menu->scroll_edificios <= -(8*64*ESCALADO_1)) 
				rec_menu->scroll_edificios += (8*64*ESCALADO_1); 
			break;
		default:
			break;
	
	}
}

void actualizar_notas(float dt, eventos_globales *ev_gl, Nota *mapa, int total_notas)
{
	int i;
	ev_gl->tiempo_juego += dt;
	float distancia_total = Y_JUEZ - Y_SPAWN;
	float velocidad = distancia_total / TIEMPO_APROXIMACION;

}
