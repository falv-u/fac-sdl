#include "commons.h"

void update(float dt, eventos_globales *ev_gl)
{
	if (ev_gl->is_iris_fading_out == true)
	{
		ev_gl->iris_radius -= 800.0*dt;
	}

	switch (ev_gl->estado_juego)
	{
		case ESTADO_JUEGO:
			break;
		default:
			break;
	
	}
}
