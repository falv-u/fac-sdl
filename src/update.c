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
			ev_gl->cae.rectangulo.y +=(int)(150.0f *dt);
			if (ev_gl->cae.rectangulo.y + ev_gl->cae.rectangulo.h >= ev_gl->base.rectangulo.y) 
                ev_gl->cae.rectangulo.y = ev_gl->base.rectangulo.y - ev_gl->cae.rectangulo.h;
			break;
		default:
			break;
	
	}
}
