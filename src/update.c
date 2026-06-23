#include "commons.h"
#include "log.h"
#include <SDL_image.h>

#define VENTANA_PERFECT 0.05f /* 50 milisegundos de tolerancia */
#define VENTANA_GOOD    0.10f /* 100 milisegundos */
#define VENTANA_MISS    0.20f /* 200 milisegundos */

void actualizar_notas(float dt, eventos_globales *ev_gl);
void update(float dt, eventos_globales *ev_gl, menu_principal_recursos *rec_menu);

void update(float dt, eventos_globales *ev_gl, menu_principal_recursos *rec_menu)
{
	if (ev_gl->is_iris_fading_out == true)
	{
		ev_gl->iris_radius -= 800.0*dt;

		if (ev_gl->iris_radius <= 0.0f && ev_gl->estado_juego == ESTADO_MENU)
		{
			ev_gl->mapa_actual = cargar_nivel_desde_lista(0);
			ev_gl->tiempo_juego = 0.0f;
			ev_gl->estado_juego = ESTADO_JUEGO;
		}
	}

	switch (ev_gl->estado_juego)
	{
		case ESTADO_MENU:
			rec_menu->scroll_edificios -= rec_menu->vel_edificios * dt;

			if (rec_menu->scroll_edificios <= -(8*64*ESCALADO_1)) 
				rec_menu->scroll_edificios += (8*64*ESCALADO_1); 
			break;

		case ESTADO_JUEGO:
			actualizar_notas(dt, ev_gl);
			break;
		default:
			break;
	
	}
}

void actualizar_notas(float dt, eventos_globales *ev_gl)
{
	ev_gl->tiempo_juego += dt;
	float velocidad = (Y_JUEZ - Y_SPAWN) / TIEMPO_APROXIMACION;

	while (ev_gl->mapa_actual.notas_pasadas < ev_gl->mapa_actual.total_notas &&
		   ev_gl->mapa_actual.arreglo_notas[ev_gl->mapa_actual.notas_pasadas].activa == false)
	{
		ev_gl->mapa_actual.notas_pasadas++;
	}

	for ( int i = ev_gl->mapa_actual.notas_pasadas; i < ev_gl->mapa_actual.total_notas; i++) 
	{
		Nota *n = &ev_gl->mapa_actual.arreglo_notas[i];

		if (n->activa == false) continue;

		float tiempo_restante = n->tiempo_golpe - ev_gl->tiempo_juego;

		if (tiempo_restante > TIEMPO_APROXIMACION) {
			n->y_actual = Y_SPAWN;
			continue; 
		}

		n->y_actual = Y_JUEZ - (velocidad * tiempo_restante);

		if ((tiempo_restante + n->duracion) < -VENTANA_MISS) {
			n->activa = false;
			game_log(LOG_INFO, "MISS - Nota perdida", 0);
		}
	}}
