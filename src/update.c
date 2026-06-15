#include "commons.h"

void update(float dt, eventos_globales *ev_gl)
{
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
