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
			break;
		default:
			break;
	
	}
}


