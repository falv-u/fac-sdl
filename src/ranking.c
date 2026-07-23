#include "ranking.h"
#include "commons.h"
#include "log.h"
#include <stdio.h>
#include <string.h>

/* NOTA: static es para limitarlas a este archivo 'ranking.c' */ 
static EntradaRanking tabla_ranking[MAX_RANKING];
static int cantidad_ranking = 0;

int ranking_obtener_cantidad(void)
{
    return cantidad_ranking;
}

void dibujar_texto(SDL_Renderer *renderizado, TTF_Font *fuente, const char *texto, int x, int y, SDL_Color color)
{
	if (fuente == NULL) return;

	SDL_Surface *surf = TTF_RenderText_Solid(fuente, texto, color);
	if (surf == NULL) return;

	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderizado, surf);
	SDL_Rect dest = { x, y, surf->w, surf->h };
	SDL_RenderCopy(renderizado, tex, NULL, &dest);

	SDL_FreeSurface(surf);
	SDL_DestroyTexture(tex);
}

void ranking_cargar(void)
{
    cantidad_ranking = 0;

    FILE *f = fopen(ARCHIVO_RANKING, "r");
    if (f == NULL) return; 

    char linea[256];
    while (cantidad_ranking < MAX_RANKING && fgets(linea, sizeof(linea), f) != NULL)
    {
        int leidos = sscanf(linea, "%31[^;];%d;%d;%d;%d;%d;%f",
            tabla_ranking[cantidad_ranking].nombre, 
            &tabla_ranking[cantidad_ranking].puntaje, 
            &tabla_ranking[cantidad_ranking].perfect, 
            &tabla_ranking[cantidad_ranking].good, 
            &tabla_ranking[cantidad_ranking].bad, 
            &tabla_ranking[cantidad_ranking].miss, 
            &tabla_ranking[cantidad_ranking].tiempo_jugado);

        if (leidos == 7)
            cantidad_ranking++;
    }

    fclose(f);
}

void ranking_guardar(void)
{
	FILE *f = fopen(ARCHIVO_RANKING, "w");
	if (f == NULL) {
		game_log(LOG_ERROR, "No se pudo guardar el ranking", ARCHIVO_RANKING);
		return;
	}

	for (int i = 0; i < cantidad_ranking; i++)
	{
		EntradaRanking *e = &tabla_ranking[i];
		fprintf(f, "%s;%d;%d;%d;%d;%d;%f\n", e->nombre, e->puntaje, e->perfect, e->good, e->bad, e->miss, e->tiempo_jugado);
	}

	fclose(f);
}

void ranking_agregar(const char *nombre, int puntaje, int perfect, int good, int bad, int miss, float tiempo_jugado)
{
	if (cantidad_ranking >= MAX_RANKING) return; /* si se llena la tabla ya no entran mas jugadores */

	EntradaRanking *e = &tabla_ranking[cantidad_ranking];

	strncpy(e->nombre, nombre, sizeof(e->nombre) - 1);
	e->nombre[sizeof(e->nombre) - 1] = '\0';
	e->puntaje = puntaje;
	e->perfect = perfect;
	e->good = good;
	e->bad = bad;
	e->miss = miss;
	e->tiempo_jugado = tiempo_jugado;

	cantidad_ranking++;
}

/* bubble sort, de mayor a menor puntaje */
void ranking_ordenar(void)
{
	for (int i = 0; i < cantidad_ranking - 1; i++)
	{
		for (int j = 0; j < cantidad_ranking - 1 - i; j++)
		{
			if (tabla_ranking[j].puntaje < tabla_ranking[j + 1].puntaje)
			{
				EntradaRanking temp = tabla_ranking[j];
				tabla_ranking[j] = tabla_ranking[j + 1];
				tabla_ranking[j + 1] = temp;
			}
		}
	}
}

void reiniciar_puntajes(event_global *ev_gl)
{
	for (int i = 0; i < 2; i++)
	{
		jugador *p = &ev_gl->jugadores[i];
		p->puntaje = 0;
		p->perfect = 0; 
		p->good = 0; 
		p->bad = 0; 
		p->miss = 0;
		p->barra_ki = 0.0f;
		p->multiplicador = 1.0f;
		p->bono_50_dado = false;
		p->bono_acumulado = 0;
		p->tiempo_x3_restante = 0.0f;
		p->tiempo_jugado = 0.0f;
		p->intensidad_pared = 0.0f;
		p->color_pared = (SDL_Color){0, 0, 0, 255};
	}

	ev_gl->nombre_ingresado[0] = '\0';
	ev_gl->jugador_pidiendo_nombre = 0;
}

void actualizar_ki_y_tiempo(event_global *ev_gl, float dt)
{
	for (int i = 0; i < 2; i++)
	{
		jugador *p = &ev_gl->jugadores[i];
		p->tiempo_jugado += dt;

		if (p->tiempo_x3_restante > 0.0f)
		{
			p->tiempo_x3_restante -= dt;
			if (p->tiempo_x3_restante <= 0.0f)
			{
				p->tiempo_x3_restante = 0.0f;
				p->barra_ki = 0.0f;
				p->bono_50_dado = false;
				p->multiplicador = 1.0f;
			}
		}
	}
}

void registrar_resultado(event_global *ev_gl, int num_jugador, RESULTADO_GOLPE tipo)
{
    jugador *p = &ev_gl->jugadores[num_jugador - 1];

    int puntos_base = 0;

    switch (tipo)
    {
        case RESULTADO_PERFECT:
            p->perfect++;
            puntos_base = 100;
            p->barra_ki += 6.0f;
            break;
        case RESULTADO_GOOD:
            p->good++;
            puntos_base = 50;
            p->barra_ki += 3.0f;
            break;
        case RESULTADO_BAD:
            p->bad++;
            puntos_base = 10;
            p->barra_ki += 1.0f;
            break;
        case RESULTADO_MISS:
            p->miss++;
            puntos_base = 0;
            p->barra_ki -= 4.0f;
            break;
    }

    if (p->barra_ki < 0.0f) p->barra_ki = 0.0f;
    if (p->barra_ki > KI_MAX) p->barra_ki = KI_MAX;

    /* Bono del 50% */
    if (p->barra_ki >= KI_UMBRAL_BONO && p->bono_50_dado == false)
    {
        p->bono_50_dado = true;
        p->bono_acumulado += 300;
    }

    /* Activar x3 al llenar */
    if (p->barra_ki >= KI_MAX && p->tiempo_x3_restante <= 0.0f)
        p->tiempo_x3_restante = KI_DURACION_X3;

    /* Actualizar multiplicador */
    if (p->tiempo_x3_restante > 0.0f)
        p->multiplicador = 3.0f;
    else if (p->barra_ki >= KI_UMBRAL_MULT)
        p->multiplicador = 1.5f;
    else
        p->multiplicador = 1.0f;

    p->puntaje += (int)(puntos_base * p->multiplicador);
}

void iniciar_flujo_fin_partida(event_global *ev_gl)
{
    if (ev_gl->jugadores[0].puntaje > 0)
    {
        ev_gl->jugador_pidiendo_nombre = 1;
        ev_gl->nombre_ingresado[0] = '\0';
        ev_gl->estado_juego = ESTADO_INGRESAR_NOMBRE;
        SDL_StartTextInput();
    }
    else if (ev_gl->jugadores[1].puntaje > 0)
    {
        ev_gl->jugador_pidiendo_nombre = 2;
        ev_gl->nombre_ingresado[0] = '\0';
        ev_gl->estado_juego = ESTADO_INGRESAR_NOMBRE;
        SDL_StartTextInput();
    }
    else
    {
        ev_gl->estado_juego = ESTADO_MENU;
    }
}

ESTADO_ACTUAL pantalla_ranking(event_global *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu)
{
	(void)evento;

	SDL_SetRenderDrawColor(ev_gl->renderizado, 10, 10, 15, 255);
	SDL_RenderClear(ev_gl->renderizado);

	dibujar_texto(ev_gl->renderizado, rec_menu->fuente, "RANKING", 550, 20, rec_menu->color2);

	if (cantidad_ranking == 0)
	{
		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, "(todavia no hay nadie en el ranking)", 300, 150, rec_menu->color1);
	}

	int inicio = ev_gl->ranking_cursor - 6;
	if (inicio < 0) inicio = 0;

	int y = 100;
	for (int i = inicio; i < cantidad_ranking && i < inicio + 12; i++)
	{
		char linea[128];
		snprintf(linea, sizeof(linea), "%2d. %-20s %d", i + 1, tabla_ranking[i].nombre, tabla_ranking[i].puntaje);

		SDL_Color color = (i == ev_gl->ranking_cursor) ? rec_menu->color2 : rec_menu->color1;
		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, linea, 250, y, color);
		y += 40;
	}

	SDL_RenderPresent(ev_gl->renderizado);
	return ESTADO_RANKING;
}

ESTADO_ACTUAL pantalla_detalle_jugador(event_global *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu)
{
	(void)evento; /* evitar warning, ignorar */

	SDL_SetRenderDrawColor(ev_gl->renderizado, 10, 10, 15, 255);
	SDL_RenderClear(ev_gl->renderizado);

	if (ev_gl->ranking_detalle_indice >= 0 && ev_gl->ranking_detalle_indice < cantidad_ranking)
	{
		EntradaRanking *e = &tabla_ranking[ev_gl->ranking_detalle_indice];
		char linea[128];

		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, e->nombre, 450, 40, rec_menu->color2);

		snprintf(linea, sizeof(linea), "Puntaje: %d", e->puntaje);
		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, linea, 250, 150, rec_menu->color1);

		snprintf(linea, sizeof(linea), "Perfect: %d", e->perfect);
		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, linea, 250, 200, rec_menu->color1);

		snprintf(linea, sizeof(linea), "Good: %d", e->good);
		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, linea, 250, 250, rec_menu->color1);

		snprintf(linea, sizeof(linea), "Bad: %d", e->bad);
		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, linea, 250, 300, rec_menu->color1);

		snprintf(linea, sizeof(linea), "Miss: %d", e->miss);
		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, linea, 250, 350, rec_menu->color1);

		int minutos = (int)e->tiempo_jugado / 60;
		int segundos = (int)e->tiempo_jugado % 60;
		snprintf(linea, sizeof(linea), "Tiempo jugado: %02d:%02d", minutos, segundos);
		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, linea, 250, 400, rec_menu->color1);

		dibujar_texto(ev_gl->renderizado, rec_menu->fuente, "(ESC para volver)", 250, 470, rec_menu->color3);
	}

	SDL_RenderPresent(ev_gl->renderizado);
	return ESTADO_DETALLE_JUGADOR;
}

ESTADO_ACTUAL pantalla_ingresar_nombre(event_global *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu)
{
	(void)evento; /*evita warning, ignorar */

	char titulo[64];
	char con_cursor[40];
	SDL_SetRenderDrawColor(ev_gl->renderizado, 10, 10, 15, 255);
	SDL_RenderClear(ev_gl->renderizado);

	snprintf(titulo, sizeof(titulo), "Jugador %d, ingresa tu nombre:", ev_gl->jugador_pidiendo_nombre);
	dibujar_texto(ev_gl->renderizado, rec_menu->fuente, titulo, 200, 250, rec_menu->color2);

	if ((SDL_GetTicks() / 400) % 2 == 0)
		snprintf(con_cursor, sizeof(con_cursor), "%s_", ev_gl->nombre_ingresado);
	else
		snprintf(con_cursor, sizeof(con_cursor), "%s", ev_gl->nombre_ingresado);

	dibujar_texto(ev_gl->renderizado, rec_menu->fuente, con_cursor, 250, 320, rec_menu->color1);

	SDL_RenderPresent(ev_gl->renderizado);
	return ESTADO_INGRESAR_NOMBRE;
}
