/* Librerias estandar */
#include <stdbool.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

/* librerias sistema no estandar */
#include <SDL_events.h> 
#include <SDL_gamecontroller.h> 
#include <SDL_keycode.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

/* headers propios */
#include "SDL_timer.h"
#include "commons.h"
#include "ranking.h"
#include "log.h"

void	 iniciar_componente(void);
void	 iniciar_recursos_menu(event_global *ev_gl);
void	 iniciar_event_global(event_global *ev_gl);
void	 manejo_delta_time(float *dt, unsigned int *l_frame, event_global *ev);
void	 manejo_estado(event_global *ev, menu_recursos *rec_menu, SDL_Event);

void	 event_global_accionados_simples(event_global *ev_gl, SDL_Event ev);
void	 eventos_menu(SDL_Event *ev, event_global *ev_gl);
void	 eventos_ranking(SDL_Event *ev, event_global *ev_gl);
void	 evento_Sel_nivel(SDL_Event *ev, event_global *ev_gl);
void	 iniciar_partida(event_global *ev_gl);
void	 eventos_accionados_usuario(event_global *ev_gl, SDL_Event ev, menu_recursos *rec_menu);

/* sub-funciones para eventos accionados por/de usuario */
void	 ingresar_nombre(event_global *ev_gl, SDL_Event ev);

void	 teclas_juego_principal(event_global *ev_gl, SDL_Event evento);
void	 evaluar_golpe(int carril_presionado, int jugador, event_global *ev_gl);

void	 manejo_mando(event_global *ev_gl, SDL_Event ev, menu_recursos *rec_menu);
void	 botones_mando_juego_principal(event_global *ev_gl, SDL_Event ev, menu_recursos *rec_menu);
void	 botones_mando_menu_principal(event_global *ev_gl, SDL_Event ev, menu_recursos *rec_menu);

bool	 es_alfombra_de_baile(SDL_GameController *mando);
Mapa	 cargar_nivel_humanizado(const char *ruta_archivo, int carril_desde, int carril_hasta);

void	 apagar_sdl(event_global *ev_gl, menu_recursos *rec_menu);

int
main(void)
{
	menu_recursos		rec_menu;	
	event_global		ev_gl;
	unsigned int		last_frame_time;
	float			delta_time;
	int			i;

	SDL_Event evento;
	srand(time(NULL));

	delta_time = 0;
	last_frame_time = SDL_GetTicks();

	iniciar_componente();
	/*
	 * la ventana tambien se crea aqui al estar
	 * anclada a los eventos globales como cerrar
	 * menu recursos esta anclado ahora a eventos globales
	 */
	iniciar_event_global(&ev_gl);

	ranking_cargar();


	while (ev_gl.sis.corriendo) {
		manejo_delta_time(&delta_time, &last_frame_time, &ev_gl);

		update(delta_time, &ev_gl,  &rec_menu);
		/*
		 * ignorar warning ya que se declara al incio de la
		 * funcion para los eventos de usuario.
		 */ 
		eventos_accionados_usuario(&ev_gl, evento, &rec_menu); 

      		manejo_estado(&ev_gl, &rec_menu, evento);

	}
	apagar_sdl(&ev_gl, &rec_menu);
	return 0;
}

void
manejo_estado(event_global *ev_gl, menu_recursos *rec_menu, SDL_Event evento)
{
	
	switch (ev_gl->estado_juego) {

	case ESTADO_MENU:
		ev_gl->estado_juego=menu_principal(ev_gl, &evento, rec_menu);
		break;
	case ESTADO_CARGA:
		ev_gl->estado_juego=pantalla_carga(ev_gl,rec_menu);
		break;
	case ESTADO_SELECT_TIPO_PARTIDA:
		break;
	case ESTADO_SELECT_NIVELES:
		ev_gl->estado_juego = seleccionar_niveles(ev_gl, rec_menu);
		break;
	case ESTADO_JUEGO:
		ev_gl->estado_juego=juego_principal(ev_gl, &evento, ev_gl->vis.fuente[0]);
		break;
	case ESTADO_GAMEOVER:
		break;
	case ESTADO_INGRESAR_NOMBRE:
		ev_gl->estado_juego = pantalla_ingresar_nombre(ev_gl, &evento, rec_menu);
		break;
	case ESTADO_RANKING:
		ev_gl->estado_juego = pantalla_ranking(ev_gl, &evento, rec_menu);
		break;
	case ESTADO_DETALLE_JUGADOR:
		ev_gl->estado_juego = pantalla_detalle_jugador(ev_gl, &evento, rec_menu);
		break;
	case ESTADO_SALIR:
		ev_gl->sis.corriendo = false;
		game_log(LOG_DEBUG, "ESTADO_SALIR HA SIDO RETORNADO POR UNA FUNCION", 0);
		break;
	default:
		break;
	}
}

void
apagar_sdl(event_global *ev_gl, menu_recursos *rec_menu)
{
	int	i;
	/*
	 * se supone que es automatico, pero es buena practica cerrar
	 * cerrar lo que abriste
	 */
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (ev_gl->par.jugadores[i].mando != NULL) {
			SDL_GameControllerClose(ev_gl->par.jugadores[i].mando);
			ev_gl->par.jugadores[i].mando = NULL;
		}
	}

	if (ev_gl->par.mapa_actual.arreglo_notas != NULL) {
		free(ev_gl->par.mapa_actual.arreglo_notas);
		ev_gl->par.mapa_actual.arreglo_notas = NULL;
	}

	if (rec_menu->textura_titulo)
		SDL_DestroyTexture(rec_menu->textura_titulo);
	if (rec_menu->sprites)
		SDL_DestroyTexture(rec_menu->sprites);
	if (rec_menu->sfx_opcion)
		Mix_FreeChunk(rec_menu->sfx_opcion);

	Mix_HaltMusic();
	if (rec_menu->musica_fondo)
		Mix_FreeMusic(rec_menu->musica_fondo);

	Mix_CloseAudio();

	if (ev_gl->vis.renderizado)
		SDL_DestroyRenderer(ev_gl->vis.renderizado);
	if (ev_gl->vis.ventana)
		SDL_DestroyWindow(ev_gl->vis.ventana);

	Mix_Quit();
	TTF_Quit();
	IMG_Quit(); 
	SDL_Quit();

	game_log(LOG_INFO, "todo ok, fin del programa...");

}

void
iniciar_recursos_menu(event_global *ev_gl)
{
	SDL_Surface	*surf;
	menu_recursos	*rec;

	rec = ev_gl->rec_menu;
	if (rec == NULL)
		return;
	
	rec->opcion=0;
	rec->sprites = IMG_LoadTexture(ev_gl->vis.renderizado, "./assets/Sprite-0001.png");

	rec->color1.r = 255;
	rec->color1.g = 255;
	rec->color1.b = 255;

	rec->color2.r = 70;
	rec->color2.g = 255;
	rec->color2.b = 0;

	rec->color3.r = 0;
	rec->color3.g = 34;
	rec->color3.b = 255;
	
	rec->musica_fondo= Mix_LoadMUS("./assets/sfx/menu.mp3");
	rec->sfx_opcion = Mix_LoadWAV("./assets/sfx/ui-menu-onset.ogg");


	rec->scroll_edificios = 0.0f;
	rec->vel_edificios = 300.0f;


	if (ev_gl->vis.fuente[0]) {
	        SDL_Surface *surf = TTF_RenderText_Solid(ev_gl->vis.fuente[0], "Feel, Amplify and Conquer!", rec->color2);
	        rec->textura_titulo = SDL_CreateTextureFromSurface(ev_gl->vis.renderizado, surf);
	        rec->titulo_w = surf->w;
	        rec->titulo_h = surf->h;
	        SDL_FreeSurface(surf);
	}
	if (rec->musica_fondo)
		Mix_PlayMusic(rec->musica_fondo, -1);
}

void
iniciar_event_global(event_global *ev_gl)
{
	int i;

	ev_gl->estado_juego = ESTADO_MENU;

	ev_gl->vis.ventana = crear_ventana();
	ev_gl->vis.renderizado = SDL_CreateRenderer(ev_gl->vis.ventana, -1, SDL_RENDERER_ACCELERATED);
	ev_gl->vis.iris_radius = 800.0;
	ev_gl->vis.is_iris_fading_out = false;
	ev_gl->vis.fuente[0] = TTF_OpenFont("./assets/fonts/CyberHorizon-ARAvL.ttf", 64);
	ev_gl->vis.fuente[1] = TTF_OpenFont("./assets/fonts/CyberHorizon-ARAvL.ttf", 64);

	SDL_RenderSetLogicalSize(ev_gl->vis.renderizado, ANCHO_PANTALLA, LARGO_PANTALLA);

	ev_gl->par.pausado = false; 
	ev_gl->par.jugadores[0].mando = NULL;
	ev_gl->par.jugadores[1].mando = NULL;
	ev_gl->par.opcion_dificultad = 0;
	ev_gl->par.musica_nivel_actual = NULL;
	ev_gl->par.jugadores[0].intensidad_pared = 0.00f;
	ev_gl->par.jugadores[1].intensidad_pared = 0.00f;


	ev_gl->sis.ranking_cursor = 0;
	ev_gl->sis.corriendo = true;
	ev_gl->sis.ranking_detalle_indice = 0;
	reiniciar_puntajes(ev_gl);

    

	ev_gl->rec_menu = malloc(sizeof(menu_recursos));
	if (ev_gl->rec_menu == NULL) {
		game_log(LOG_ERROR, "NO SE ASIGNO MEMORIA A RECURSOS DEL MENU, FALLO CRITICO");
		return;
	}

	iniciar_recursos_menu(ev_gl);

	for (i = 0; i < SDL_NumJoysticks(); i++) {
		if (!SDL_IsGameController(i))
			continue;
		ev_gl->par.jugadores[0].mando = SDL_GameControllerOpen(i);
		if (ev_gl->par.jugadores[0].mando == NULL)
			continue;
		game_log(LOG_INFO, "Mando pre-conectado detectado y asignado");
	}
}

void
event_global_accionados_simples(event_global *ev_gl, SDL_Event evento)
{
	if (evento.type == SDL_QUIT) {
		game_log(LOG_INFO, "Cerrando ventana por accion del usuario a traves del servidor grafico ");
		ev_gl->sis.corriendo = false;
	}

	if (evento.type == SDL_KEYDOWN &&  evento.key.repeat == 0) {
		if (evento.key.keysym.sym == SDLK_F9)
			Pantalla_Completa(ev_gl->vis.ventana);
		if (evento.key.keysym.sym == SDLK_ESCAPE) {
			game_log(LOG_DEBUG, "cierre por fin del loop a traves de la bandera 'corriendo', accionado por tecla ESCAPE");
			ev_gl->sis.corriendo = false;
		}
	}

}

void
iniciar_componente(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) 
		game_log(LOG_ERROR, "SDL no pudo inicializarse! Error: %s\n", SDL_GetError());
	/* base de datos sdl comunitaria para dispositivos plug */
	if (SDL_GameControllerAddMappingsFromFile("./gamecontrollerdb.txt") < 0)
		game_log(LOG_WARN, "No se encontro gamecontrollerdb.txt o hubo un error %s\n", SDL_GetError());
	
	if (TTF_Init() == -1) 
		game_log(LOG_ERROR, "SDL_ttf error: %s\n", TTF_GetError());

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		printf("Error: %s\n", Mix_GetError());

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	game_log(LOG_DEBUG, "Componentes de SDL iniciados correctamente!\n");
}

void
manejo_delta_time(float *dt, unsigned int *last_frame_time, event_global *ev_gl)
{
	unsigned int current_time = SDL_GetTicks();
	unsigned int frame_duration = SDL_GetTicks() - current_time;

	*dt = (current_time - *last_frame_time) / 1000.0f;

	*last_frame_time = SDL_GetTicks();

	if (*dt > 0.1f)
		*dt = 0.1f;

	*last_frame_time = current_time;	

	if (frame_duration < FRAME_TARGET_TIME)
		SDL_Delay(FRAME_TARGET_TIME - frame_duration);

	if (ev_gl->estado_juego == ESTADO_JUEGO && ev_gl->par.pausado == true)
		*dt = 0.0f;
}


void
eventos_menu(SDL_Event *ev, event_global *ev_gl)
{
	menu_recursos	*rec;
	int		opcion_anterior;

	rec = ev_gl->rec_menu;
	opcion_anterior = rec->opcion;

	if ( ev->type != SDL_KEYDOWN ) 
		return;

	switch (ev->key.keysym.sym) {
	case SDLK_UP:
		if ( rec->opcion > 0 )
			rec->opcion--;
		else
			rec->opcion = 2;
	case SDLK_DOWN:
		if ( rec->opcion < 2 )
			rec->opcion++;
		else
			rec->opcion = 0;
	case SDLK_RETURN:
		switch (rec->opcion) {
		case 0:
			ev_gl->estado_juego = ESTADO_SELECT_NIVELES;
			break;
		case 1:
			ev_gl->estado_juego = ESTADO_SALIR;
			break;
		case 2:
			ev_gl->estado_juego = ESTADO_RANKING;
			break;
		default:
			break;
		}
		break;
	default:
		return;
	}

	if (opcion_anterior != rec->opcion && rec->sfx_opcion != NULL)
		Mix_PlayChannel(-1, rec->sfx_opcion, 0);
}
void
iniciar_partida(event_global *ev_gl)
{
	generar_playlist_aleatoria(ev_gl, ev_gl->par.opcion_dificultad + 1);
	reiniciar_puntajes(ev_gl);
        if ( ev_gl->sis.playlist.modo_playlist && ev_gl->sis.playlist.cantidad > 0 ) {
		if (es_alfombra_de_baile(ev_gl->par.jugadores[0].mando))
			ev_gl->par.mapa_actual = cargar_nivel_humanizado(ev_gl->sis.playlist.rutas[0], 0, 3);
		else
			ev_gl->par.mapa_actual = cargar_nivel(ev_gl->sis.playlist.rutas[0]);

		ev_gl->par.tiempo_juego = 0.0f;
		ev_gl->par.musica_iniciada = false;
		ev_gl->estado_juego = ESTADO_CARGA;

		game_log(LOG_INFO, "Iniciando partida aleatoria.");

		if (ev_gl->par.musica_nivel_actual != NULL) {
			Mix_HaltMusic();
			Mix_FreeMusic(ev_gl->par.musica_nivel_actual);
		}

		ev_gl->par.musica_nivel_actual = Mix_LoadMUS(ev_gl->sis.playlist.rutas_audio[0]);

		} else {
			ev_gl->estado_juego = ESTADO_MENU; /* resguardo si la carpeta esta vacia */
		}
}

void
evento_Sel_nivel(SDL_Event *ev, event_global *ev_gl)
{
	if ( ev->type != SDL_KEYDOWN ) 
		return;

	switch (ev->key.keysym.sym){
	case SDLK_UP:
		if ( ev_gl->par.opcion_dificultad > 0 )
			ev_gl->par.opcion_dificultad--;
		break;
	case SDLK_DOWN:
		if ( ev_gl->par.opcion_dificultad < 3) 
			ev_gl->par.opcion_dificultad++;
		break;
	case SDLK_RETURN:
		iniciar_partida(ev_gl);
		break;
	default:
		break;
	}
}

void
eventos_ranking(SDL_Event *ev, event_global *ev_gl)
{
	int total_elementos;

	if ( ev->type != SDL_KEYDOWN ) 
		return;
	total_elementos = ranking_obtener_cantidad();
	switch (ev->key.keysym.sym){
	case SDLK_UP:
		if (ev_gl->sis.ranking_cursor > 0 ) 
			ev_gl->sis.ranking_cursor--;
		break;
	case SDLK_DOWN:
	   if (total_elementos > 0 &&  ev_gl->sis.ranking_cursor < total_elementos - 1)
			ev_gl->sis.ranking_cursor++;
		break;
	case SDLK_RETURN:
		if ( total_elementos > 0) {
		        ev_gl->sis.ranking_detalle_indice = ev_gl->sis.ranking_cursor;
		        ev_gl->estado_juego = ESTADO_DETALLE_JUGADOR;
	        }
		break;
	case SDLK_BACKSPACE:
		ev_gl->estado_juego = ESTADO_MENU;
		break;
	case SDLK_LEFT:
		ev_gl->estado_juego = ESTADO_MENU;
		break;
	default:
		break;
	}
}
void
eventos_accionados_usuario(event_global *ev_gl, SDL_Event evento, menu_recursos *rec_menu)
{

	while (SDL_PollEvent(&evento)) {
		event_global_accionados_simples(ev_gl,evento);
		manejo_mando(ev_gl, evento, rec_menu);

		if (evento.type == SDL_KEYDOWN && evento.key.repeat == 0 || evento.type == SDL_CONTROLLERBUTTONDOWN) {
			ingresar_nombre(ev_gl, evento);
			if (ev_gl->estado_juego == ESTADO_MENU) 
				eventos_menu(&evento, ev_gl);
			if (ev_gl->estado_juego == ESTADO_JUEGO)
				teclas_juego_principal(ev_gl, evento);

			if (ev_gl->estado_juego == ESTADO_SELECT_NIVELES)
				evento_Sel_nivel(&evento, ev_gl);
			if (ev_gl->estado_juego == ESTADO_RANKING)
				eventos_ranking(&evento, ev_gl);
		}
	}
}


void
ingresar_nombre(event_global *ev_gl, SDL_Event ev)
{

	jugador	*p1;  
	jugador *p2;  
	size_t	 len; /* funciones strlen devuelven size_t*/

	if (ev_gl->estado_juego != ESTADO_INGRESAR_NOMBRE)
		return;

	if (ev.type == SDL_TEXTINPUT) {
		if (strlen(ev_gl->sis.nombre_ingresado) + strlen(ev.text.text) < sizeof(ev_gl->sis.nombre_ingresado) - 1)
			strcat(ev_gl->sis.nombre_ingresado, ev.text.text);
		return;
	}

	if (ev.type != SDL_TEXTINPUT)
		return;

	len =  strlen(ev_gl->sis.nombre_ingresado);

	if (ev.key.keysym.sym == SDLK_BACKSPACE && len > 0) {
		ev_gl->sis.nombre_ingresado[len - 1] = '\0';
		return;
	}

	if (ev.key.keysym.sym != SDLK_RETURN || len == 0)
		return;
	
	p1= &ev_gl->par.jugadores[0];
	p2= &ev_gl->par.jugadores[1];

	if (ev_gl->sis.jugador_pidiendo_nombre == 1) {
		ranking_agregar(ev_gl->sis.nombre_ingresado,
		                p1->puntaje + p1->bono_acumulado,
				p1->perfect, p1->good, p1->bad,
				p1->miss, p1->tiempo_jugado);

		if (ev_gl->par.jugadores[1].puntaje >  0) {
			ev_gl->sis.jugador_pidiendo_nombre = 2;
			ev_gl->sis.nombre_ingresado[0] = '\0';
			return;
		}

	} else if (ev_gl->sis.jugador_pidiendo_nombre == 2) {
		ranking_agregar(ev_gl->sis.nombre_ingresado,
				p2->puntaje + p2->bono_acumulado,
				p2->perfect,p2->good, p2->bad,
				p2->miss, p2->tiempo_jugado);
	}
	
	SDL_StopTextInput();
	ranking_ordenar();
	ranking_guardar();
	ev_gl->sis.ranking_cursor = 0;
	ev_gl->estado_juego = ESTADO_RANKING;
}


void
activar_efecto_pared(event_global *ev_gl, int num_jugador, int resultado) 
{
    jugador *p = &ev_gl->par.jugadores[num_jugador - 1];
    
    p->intensidad_pared = 1.0f; /* Máximo brillo */
    
    if (resultado == RESULTADO_PERFECT) {
        if (num_jugador == 1)
        	p->color_pared = (SDL_Color){0, 255, 255, 255};
        else
        	 p->color_pared = (SDL_Color){255, 0, 255, 255}; /* Magenta P2 */
    } 
    else if (resultado == RESULTADO_GOOD) {
        p->color_pared = (SDL_Color){50, 205, 50, 255}; /* Verde */
    } 
    else if (resultado == RESULTADO_MISS) {
        p->color_pared = (SDL_Color){255, 30, 30, 255}; /* Rojo */
    }
}
void
evaluar_golpe(int carril_presionado, int jugador, event_global *ev_gl)
{

	Nota *n;
	float delta_t;
	char msg[64];
	int i;

    for ( i = ev_gl->par.mapa_actual.notas_pasadas; i < ev_gl->par.mapa_actual.total_notas; i++)
    {
	n = &ev_gl->par.mapa_actual.arreglo_notas[i];

	if (n->activa == false || n->carril != carril_presionado)
		continue;

	delta_t = (float)fabs(n->tiempo_golpe - ev_gl->par.tiempo_juego);

	if (delta_t > 0.20f && n->tiempo_golpe > ev_gl->par.tiempo_juego)
		break;

	if (delta_t <= 0.05f) {
		snprintf(msg, sizeof(msg), "[P%d] PERFECT", jugador);
		game_log(LOG_INFO, msg, 0);
		registrar_resultado(ev_gl, jugador, RESULTADO_PERFECT);
		activar_efecto_pared(ev_gl, jugador, RESULTADO_PERFECT); 
		n->activa = false; 
		break;
        } else if (delta_t <= 0.10f) {
		snprintf(msg, sizeof(msg), "[P%d] GOOD", jugador);
		game_log(LOG_INFO, msg, 0);
		registrar_resultado(ev_gl, jugador, RESULTADO_GOOD);
		activar_efecto_pared(ev_gl, jugador, RESULTADO_GOOD); 
		n->activa = false;
		break;
        } else if (delta_t <= 0.20f) {
		snprintf(msg, sizeof(msg), "[P%d] BAD", jugador);
		game_log(LOG_INFO, msg, 0);
		registrar_resultado(ev_gl, jugador, RESULTADO_BAD);
		activar_efecto_pared(ev_gl, jugador, RESULTADO_BAD); 
		n->activa = false;
		break;
        }
    }
}

void
teclas_juego_principal(event_global *ev_gl, SDL_Event evento)
{
	int carril_presionado = -1;
	int jugador = -1;

	if (evento.key.keysym.sym == SDLK_p)
	{
		ev_gl->par.pausado = !ev_gl->par.pausado;
		if (ev_gl->par.pausado)
		{
			Mix_PauseMusic();
		} else {
			Mix_ResumeMusic();
		}
		return;
	}
	if (ev_gl->par.pausado) return;
	/* Carriles P1 */
	if (evento.key.keysym.sym == SDLK_h) {
		 carril_presionado = 0;
		 jugador = 1;
	}
	else if (evento.key.keysym.sym == SDLK_j) { carril_presionado = 1; jugador = 1; }
	else if (evento.key.keysym.sym == SDLK_k) { carril_presionado = 2; jugador = 1; }
	else if (evento.key.keysym.sym == SDLK_l) { carril_presionado = 3; jugador = 1; }

	/* Carriles P2 */
	else if (evento.key.keysym.sym == SDLK_q) { carril_presionado = 4; jugador = 2; }
	else if (evento.key.keysym.sym == SDLK_w) { carril_presionado = 5; jugador = 2; }
	else if (evento.key.keysym.sym == SDLK_e) { carril_presionado = 6; jugador = 2; }
	else if (evento.key.keysym.sym == SDLK_r) { carril_presionado = 7; jugador = 2; }

	if (carril_presionado != -1)
		evaluar_golpe(carril_presionado, jugador, ev_gl);
}


void
manejo_mando(event_global *ev_gl, SDL_Event evento, menu_recursos *rec_menu)
{
	if (evento.type == SDL_CONTROLLERDEVICEADDED) 
	{
		SDL_GameController *mando_temporal = SDL_GameControllerOpen(evento.cdevice.which);
		if (mando_temporal) 
		{
			const char *nombre = SDL_GameControllerName(mando_temporal);

			if (nombre && strstr(nombre, "Microntek") != NULL) 
			{
				if (ev_gl->par.jugadores[0].mando == NULL)
				{
					ev_gl->par.jugadores[0].mando = mando_temporal;
					game_log(LOG_INFO, "Alfombra asignada a Jugador 1", nombre);
				} else {
					SDL_GameControllerClose(mando_temporal);
				}
			} 
			else {
				if (ev_gl->par.jugadores[1].mando == NULL) {
					ev_gl->par.jugadores[1].mando = mando_temporal;
					game_log(LOG_INFO, "Mando convencional asignado a Jugador 2", nombre);
				} else {
					SDL_GameControllerClose(mando_temporal);
				}
			}
		}
	} 
	else if (evento.type == SDL_CONTROLLERDEVICEREMOVED) {
		SDL_GameController *mando_desconectado = SDL_GameControllerFromInstanceID(evento.cdevice.which);
		if (ev_gl->par.jugadores[0].mando == mando_desconectado) {
			SDL_GameControllerClose(ev_gl->par.jugadores[0].mando);
			ev_gl->par.jugadores[0].mando = NULL;
			game_log(LOG_INFO, "Alfombra del Jugador 1 desconectada", 0);
		}
		else if (ev_gl->par.jugadores[1].mando == mando_desconectado) {
			SDL_GameControllerClose(ev_gl->par.jugadores[1].mando);
			ev_gl->par.jugadores[1].mando = NULL;
			game_log(LOG_INFO, "Mando del Jugador 2 desconectado");
		}
	}

	if (evento.type == SDL_CONTROLLERBUTTONDOWN)
	{
		if (ev_gl->estado_juego == ESTADO_JUEGO)
			botones_mando_juego_principal(ev_gl, evento, rec_menu);
		else if (ev_gl->estado_juego == ESTADO_MENU)
			botones_mando_menu_principal(ev_gl, evento, rec_menu);
	}
}

void botones_mando_juego_principal(event_global
                                 *ev_gl, SDL_Event evento, menu_recursos *rec_menu)
{
	int carril_presionado = -1;
	int offset_carril = -1;
	int jugador = -1;
	if (ev_gl->par.jugadores[0].mando && evento.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(ev_gl->par.jugadores[0].mando))) {
		offset_carril = 0;
		jugador = 1;
	} 
	else if (ev_gl->par.jugadores[1].mando && evento.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(ev_gl->par.jugadores[1].mando))) {
		offset_carril = 4;
		jugador = 2;
	}

    if (offset_carril == -1) return;
	switch (evento.cbutton.button) 
	{
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		carril_presionado = 0; 
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_UP:   
		carril_presionado = 1; 
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:         
		carril_presionado = 2; 
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:         
		carril_presionado = 3; 
		break;

	case SDL_CONTROLLER_BUTTON_X:
		carril_presionado = 0; 
		break;
	case SDL_CONTROLLER_BUTTON_Y:   
		carril_presionado = 1; 
		break;
	case SDL_CONTROLLER_BUTTON_A:         
		carril_presionado = 2; 
		break;
	case SDL_CONTROLLER_BUTTON_B:         
		carril_presionado = 3; 
	}
	if (carril_presionado != -1)
	        evaluar_golpe(carril_presionado + offset_carril,jugador, ev_gl);
	
}

void botones_mando_menu_principal(event_global *ev_gl, SDL_Event evento, menu_recursos *rec_menu)
{
	int opcion_anterior = rec_menu->opcion;

	switch (evento.cbutton.button) 
	{
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		if (rec_menu->opcion > 0) rec_menu->opcion--;
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		if (rec_menu->opcion < 3) rec_menu->opcion++;
		break;
	case SDL_CONTROLLER_BUTTON_START:
		if (rec_menu->opcion == 0)
			ev_gl->vis.is_iris_fading_out = true;
		else if (rec_menu->opcion == 2) {
			ev_gl->sis.ranking_cursor = 0;
			ev_gl->estado_juego = ESTADO_RANKING;
		}
		break;
	}

	if (rec_menu->opcion > 2)
		rec_menu->opcion = 0; 
	if (rec_menu->opcion < 0) 
		rec_menu->opcion = 2;

	if(opcion_anterior != rec_menu->opcion)
	{
		if (rec_menu->sfx_opcion)
			Mix_PlayChannel(-1, rec_menu->sfx_opcion, 0);
	}
}
