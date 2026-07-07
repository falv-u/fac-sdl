#include "commons.h"
#include "log.h"
#include <SDL_image.h>
#include <dirent.h>
#define VENTANA_PERFECT 0.05f /* 50 milisegundos de tolerancia */
#define VENTANA_GOOD    0.10f /* 100 milisegundos */
#define VENTANA_MISS    0.20f /* 200 milisegundos */

void actualizar_notas(float dt, eventos_globales *ev_gl);
void update(float dt, eventos_globales *ev_gl, menu_principal_recursos *rec_menu);

MapaCancion cargar_nivel(const char *ruta_archivo);

void update(float dt, eventos_globales *ev_gl, menu_principal_recursos *rec_menu)
{
	if (ev_gl->is_iris_fading_out == true)
	{
	    ev_gl->iris_radius -= 800.0 * dt;

	    if (ev_gl->iris_radius <= 0.0f)
	    {
	        ev_gl->is_iris_fading_out = false;
	        ev_gl->iris_radius = 800.0f;
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

MapaCancion cargar_nivel(const char *ruta_archivo)
{
    MapaCancion mapa = {0};
    if (strlen(ruta_archivo) == 0) return mapa;

    FILE *archivo = fopen(ruta_archivo, "r");
    char linea[128];

    if (!archivo) {
        game_log(LOG_ERROR, "No se pudo abrir el mapa", ruta_archivo);
        return mapa;
    }

    while (fgets(linea, sizeof(linea), archivo)) {
        if (linea[0] != '#' && linea[0] != '\n') mapa.total_notas++;
    }

    mapa.arreglo_notas = (Nota *)malloc(sizeof(Nota) * mapa.total_notas);
    rewind(archivo);

    int indice = 0;
    while (fgets(linea, sizeof(linea), archivo)) 
    {
        if (linea[0] == '#' || linea[0] == '\n') continue;

        float t_golpe, duracion;
        int carril;
        
        if (sscanf(linea, "%f;%f;%d", &t_golpe, &duracion, &carril) == 3) 
        {
            if (duracion > 0.0f) 
            {
                int largas_activas = 0;
                for (int j = 0; j < indice; j++) {
                    if (mapa.arreglo_notas[j].duracion > 0.0f) {
                        float fin_previa = mapa.arreglo_notas[j].tiempo_golpe + mapa.arreglo_notas[j].duracion;
                        if (t_golpe < fin_previa) largas_activas++; 
                    }
                }
                
                if (largas_activas >= 2) {
                    duracion = 0.0f;
                }
            }

            if (carril == -1) 
            {
                int candidato;
                while (1) {
                    candidato = rand() % 8; 
                    int choca = 0;
                    
                    for (int j = 0; j < indice; j++) {
                        float fin_previa = mapa.arreglo_notas[j].tiempo_golpe + mapa.arreglo_notas[j].duracion;
                        if (t_golpe < fin_previa && mapa.arreglo_notas[j].carril == candidato) {
                            choca = 1; 
                            break;
                        }
                    }
                    if (!choca) break; 
                }
                carril = candidato;
            }

            mapa.arreglo_notas[indice].tiempo_golpe = t_golpe;
            mapa.arreglo_notas[indice].duracion = duracion;
            mapa.arreglo_notas[indice].carril = carril;
            mapa.arreglo_notas[indice].activa = true;
            mapa.arreglo_notas[indice].y_actual = -100.0f;
            indice++;
        }
    }

    fclose(archivo);
    return mapa;
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

		if ((tiempo_restante + n->duracion) < -VENTANA_MISS)
		{
			n->activa = false;

			/* Deducir el jugador según la posición lógica del carril */
			int jugador_miss = (n->carril < 4) ? 1 : 2;
			char msg[64];

			snprintf(msg, sizeof(msg), "[P%d] MISS - Nota perdida", jugador_miss);
			game_log(LOG_INFO, msg, 0);
        	}
	}

		if (ev_gl->mapa_actual.total_notas > 0 && ev_gl->mapa_actual.notas_pasadas >= ev_gl->mapa_actual.total_notas) 
		{
		    /* Tolerancia de 2.0 segundos despues de la última nota para que no corte abruptamente */
		    float tiempo_ultima_nota = ev_gl->mapa_actual.arreglo_notas[ev_gl->mapa_actual.total_notas - 1].tiempo_golpe;
  
		    if (ev_gl->tiempo_juego > (tiempo_ultima_nota + 2.0f)) 
		    {
		        if (ev_gl->mapa_actual.arreglo_notas != NULL) {
		            free(ev_gl->mapa_actual.arreglo_notas);
		            ev_gl->mapa_actual.arreglo_notas = NULL;
		        }
		        ev_gl->mapa_actual.total_notas = 0;
		        ev_gl->mapa_actual.notas_pasadas = 0;

		        if (ev_gl->playlist.modo_playlist) {
		            ev_gl->playlist.actual++;

		            if (ev_gl->playlist.actual < ev_gl->playlist.cantidad) {
		                ev_gl->mapa_actual = cargar_nivel(ev_gl->playlist.rutas[ev_gl->playlist.actual]);
		                ev_gl->tiempo_juego = 0.0f;
		                game_log(LOG_INFO, "Cargando siguiente mapa de la playlist aleatoria.", 0);
		            } else {
		                ev_gl->playlist.modo_playlist = false;
		                ev_gl->estado_juego = ESTADO_MENU;
		                game_log(LOG_INFO, "¡Partida aleatoria completada exitosamente!", 0);
		            }
		        } else {
		            ev_gl->estado_juego = ESTADO_MENU;
		        }
		    }
		}
}

