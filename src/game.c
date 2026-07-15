#include <math.h>
#include "SDL_ttf.h"
#include "commons.h"
#include "ranking.h"

/*solo estetica, no afecta jugabilidad */
void dibujar_barras(eventos_globales *);
void grid_pseudo3d(eventos_globales *);

void renderizar_texto_juego(SDL_Renderer *renderizado, TTF_Font *fuente, const char *texto, int x, int y, SDL_Color color);
void dibujar_marcadores(eventos_globales *ev_gl, TTF_Font *fuente);

ESTADO_ACTUAL juego_principal(eventos_globales *ev_gl, SDL_Event *evento, TTF_Font *fuente)
{

	(void)evento; /* evita warning, ignora */
	

  SDL_SetRenderDrawColor(ev_gl->renderizado, 15, 15, 20, 255);
	SDL_RenderClear(ev_gl->renderizado);
	dibujar_barras(ev_gl);
	grid_pseudo3d(ev_gl);
	
	/* Calculamos el margen izquierdo para centrar 8 carriles de 64px */
	int x_base = (int)((ANCHO_PANTALLA / 2) - ((8 * 64 + 64) / 2));

	/* dibujar linae juez */
	SDL_SetRenderDrawColor(ev_gl->renderizado, 100, 100, 100, 255); 
	for (int carril = 0; carril < 8; carril++) 
	{
		int x_carril_juez = x_base + (carril * 64);
		if (carril >= 4) x_carril_juez += 64; /* Espacio de separación P1 y P2 */

		SDL_Rect rect_juez = { x_carril_juez, (int)Y_JUEZ-16, 64, 32 };
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

		float velocidad = (Y_JUEZ - Y_SPAWN) / TIEMPO_APROXIMACION;

		/* Altura base para nota normal (32px), si es larga, escalamos por el tiempo */
		int alto_primitiva = (n->duracion > 0.0f) ? (int)(velocidad * n->duracion) : 32;

		/* El ancho dinamico considerando que sera de dos players */
		int x_carril = x_base + (n->carril * 64);

		if (n->carril >= 4)
			x_carril += 64; 

		SDL_Rect rect_nota = {
			x_carril,
			(int)(n->y_actual - alto_primitiva + 16), 
			64,
			alto_primitiva
		};		
		SDL_RenderFillRect(ev_gl->renderizado, &rect_nota);
	}

	dibujar_marcadores(ev_gl, fuente);
	SDL_RenderPresent(ev_gl->renderizado);
	return ESTADO_JUEGO;
}

void dibujar_barras(eventos_globales *ev_gl)
{
    SDL_SetRenderDrawBlendMode(ev_gl->renderizado, SDL_BLENDMODE_BLEND);

    int num_barras = 16; /* Cantidad de barras verticales */
    int alto_barra = (LARGO_PANTALLA / num_barras);
    int ancho_base = 90; 

    for (int p = 0; p < 2; p++)
    {
        jugador *jug = &ev_gl->jugadores[p];

        if (jug->intensidad_pared > 0.0f) {
            SDL_SetRenderDrawColor(ev_gl->renderizado, 
                                   jug->color_pared.r, 
                                   jug->color_pared.g, 
                                   jug->color_pared.b, 
                                   (Uint8)(150 * jug->intensidad_pared));
                               
            for (int i = 0; i < num_barras; i++) {
                /* alternar el ancho de las barras para que parezca un ecualizador */
                int variacion = 0;
                if (i % 2 == 0) variacion = 15;
                else if (i % 3 == 0) variacion = -10;
            
                int ancho_real = (int)((ancho_base + variacion) * jug->intensidad_pared);
                
                SDL_Rect barra;
                if (p == 0) {
                    /* Pared P1 (Izquierda) */
                    barra = (SDL_Rect){ 0, (i * alto_barra) + 2, ancho_real, alto_barra - 4 };
                } else {
                    /* Pared P2 (Derecha) */
                    barra = (SDL_Rect){ ANCHO_PANTALLA - ancho_real, (i * alto_barra) + 2, ancho_real, alto_barra - 4 };
                }

                SDL_RenderFillRect(ev_gl->renderizado, &barra);
            }
        }
    }

    SDL_SetRenderDrawBlendMode(ev_gl->renderizado, SDL_BLENDMODE_NONE);
}

void grid_pseudo3d(eventos_globales *ev_gl)
{
	SDL_SetRenderDrawBlendMode(ev_gl->renderizado, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(ev_gl->renderizado, 150, 0, 150, 100); /* Violeta translúcido */

	int horizonte_y = LARGO_PANTALLA / 2;
	int centro_x = ANCHO_PANTALLA / 2;

	/* Iteramos desde -15 hasta 15 para abrir el angulo 15 grados y se vaya de la pantalla..creo que era asi */
	for (int i = -15; i <= 15; i++)
	{
	    int x_fondo = centro_x + (i * 150);
	    SDL_RenderDrawLine(ev_gl->renderizado, centro_x, horizonte_y, x_fondo, LARGO_PANTALLA);
	}

	float velocidad = 40.0f; /* Velocidad de desplazamiento hacia el jugador (cuarta pared)*/
	float espacio_z = 10.0f; /* Distancia entre cada línea en el mundo 3D */
	float z_max = 200.0f;    /* Profundidad max antes de que desaparezcan en el horizonte */

	/* offset_z genera el bucle: fmodf asegura que el valor rote entre 0 y espacio_z */
	float offset_z = fmodf(ev_gl->tiempo_juego * velocidad, espacio_z);

	for (float z = 1.0f; z < z_max; z += espacio_z) {
	    float z_actual = z - offset_z;
    
	    /* Evitar división por cero */
	    if (z_actual <= 0.1f)
	    	 continue; 

	    /* Proyección Z: La posición Y en pantalla es el horizonte mas una escala dividida por la distancia*/
	    /* Mientras mayor es Z mas pequeño es el resultado, pegando la linea al horizonte. */
	    int y_pantalla = horizonte_y + (int)(800.0f / z_actual);

	    /* Recortar para no dibujar sobre los marcadores superiores ni fuera de la ventana */
	    if (y_pantalla < LARGO_PANTALLA && y_pantalla >= horizonte_y) {
	        SDL_RenderDrawLine(ev_gl->renderizado, 0, y_pantalla, ANCHO_PANTALLA, y_pantalla);
	    }
	}
	SDL_SetRenderDrawBlendMode(ev_gl->renderizado, SDL_BLENDMODE_NONE);
}

void renderizar_texto_juego(SDL_Renderer *renderizado, TTF_Font *fuente, const char *texto, int x, int y, SDL_Color color)
{
	if (fuente == NULL)
		return;

	SDL_Surface *surf = TTF_RenderText_Solid(fuente, texto, color);
	if (surf == NULL)
		return;

	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderizado, surf);
	SDL_Rect dest = { x, y, surf->w, surf->h };
	SDL_RenderCopy(renderizado, tex, NULL, &dest);

	SDL_FreeSurface(surf);
	SDL_DestroyTexture(tex);
}

void dibujar_marcadores(eventos_globales *ev_gl, TTF_Font *fuente)
{
    char buffer_numeros[32];
    SDL_Color color_blanco = { 255, 255, 255, 255 };
    SDL_Color color_bono = { 70, 255, 0, 255 };
    int ancho_barra = 300;
    int alto_barra = 24;

    for (int p = 0; p < 2; p++)
    {
        jugador *jug = &ev_gl->jugadores[p];

        int x_barra;
        int x_puntaje;
        int x_bono;

        if (p == 0) {
            /* Jugador 1 (Izquierda) */
            x_barra = 20;
            x_puntaje = 335;
            x_bono = 450;
        } else {
            /* Jugador 2 (Derecha) */
            x_barra = ANCHO_PANTALLA - 20 - ancho_barra;
            x_puntaje = x_barra - 90;
            x_bono = x_barra - 240;
        }

        SDL_Rect fondo = { x_barra, 20, ancho_barra, alto_barra };
        SDL_SetRenderDrawColor(ev_gl->renderizado, 60, 60, 60, 255);
        SDL_RenderDrawRect(ev_gl->renderizado, &fondo);

        int relleno = (int)(ancho_barra * (jug->barra_ki / KI_MAX));
        SDL_Rect relleno_rect = { x_barra, 20, relleno, alto_barra };

        if (jug->tiempo_x3_restante > 0.0f) {
            SDL_SetRenderDrawColor(ev_gl->renderizado, 255, 40, 40, 255); /* Rojo (Multiplicador x3 activo) */
        } else if (jug->barra_ki >= KI_UMBRAL_MULT) {
            SDL_SetRenderDrawColor(ev_gl->renderizado, 255, 140, 0, 255); /* Naranja (Cerca del máximo) */
        } else if (jug->barra_ki >= KI_UMBRAL_BONO) {
            SDL_SetRenderDrawColor(ev_gl->renderizado, 255, 255, 0, 255); /* Amarillo (Umbral superado) */
        } else {
            /* Color base según el jugador (Cyan para P1, Magenta para P2) */
            if (p == 0) {
                SDL_SetRenderDrawColor(ev_gl->renderizado, 0, 255, 255, 255);
            } else {
                SDL_SetRenderDrawColor(ev_gl->renderizado, 255, 0, 255, 255);
            }
        }

        SDL_RenderFillRect(ev_gl->renderizado, &relleno_rect);

        snprintf(buffer_numeros, sizeof(buffer_numeros), "%d", jug->puntaje);
        renderizar_texto_juego(ev_gl->renderizado, fuente, buffer_numeros, x_puntaje, 15, color_blanco);

        snprintf(buffer_numeros, sizeof(buffer_numeros), "Bono: %d", jug->bono_acumulado);
        renderizar_texto_juego(ev_gl->renderizado, fuente, buffer_numeros, x_bono, 15, color_bono);
    }
}
