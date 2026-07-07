/* Librerias estandar */
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* librerias sistema no estandar */
/* headers propios */
#include "SDL_events.h" 
#include "SDL_gamecontroller.h" 
#include "SDL_keycode.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "commons.h"
#include "log.h"

void iniciar_componente();
void iniciar_recursos_menu(menu_principal_recursos *rec_menu, eventos_globales *ev_gl);
void iniciar_eventos_globales(eventos_globales *ev_gl);
void manejo_delta_time(float *delta_time, unsigned int *last_frame_time);

void eventos_globales_accionados_simples(eventos_globales *ev_gl, SDL_Event evento);
void eventos_accionados_usuario(eventos_globales *ev_gl, SDL_Event evento, menu_principal_recursos *rec_menu);
void teclas_menu_principal(menu_principal_recursos *rec_menu, SDL_Event evento);


void teclas_juego_principal(eventos_globales *ev_gl, SDL_Event evento);
void evaluar_golpe(int carril_presionado, int jugador, eventos_globales *ev_gl);

void manejo_mando(eventos_globales *ev_gl, SDL_Event evento, menu_principal_recursos *rec_menu);
void botones_mando_juego_principal(eventos_globales *ev_gl, SDL_Event evento, menu_principal_recursos *rec_menu);
void botones_mando_menu_principal(eventos_globales *ev_gl, SDL_Event evento, menu_principal_recursos *rec_menu);

int main(void)
{
	srand(time(NULL));
	eventos_globales ev_gl;
	menu_principal_recursos rec_menu;
	
	float delta_time=0;
	unsigned int last_frame_time = SDL_GetTicks();

	iniciar_componente();
	iniciar_eventos_globales(&ev_gl); /* la ventana tambien se crea aqui al estar anclada a los eventos globales como cerrar */
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
	    if (SDL_IsGameController(i)) {
	        ev_gl.mando_p1 = SDL_GameControllerOpen(i);
	        if (ev_gl.mando_p1) {
	            game_log(LOG_INFO, "Mando pre-conectado detectado y asignado", SDL_GameControllerName(ev_gl.mando_p1));
	            break; 
	        }
	    }
	}

	iniciar_recursos_menu(&rec_menu, &ev_gl);

	while (ev_gl.corriendo)
	{
		SDL_Event evento;

		manejo_delta_time(&delta_time, &last_frame_time);
		update(delta_time, &ev_gl,  &rec_menu);

		/* ignorar warning ya que se declara al incio de la funcion para los eventos de usuario */ 
		eventos_accionados_usuario(&ev_gl, evento, &rec_menu); 
      
		switch (ev_gl.estado_juego)
			{
				case ESTADO_MENU:
					ev_gl.estado_juego=menu_principal(&ev_gl, &evento, &rec_menu);
					break;
				case ESTADO_CARGA:
					break;
				case ESTADO_PAUSA:
					break;
				case ESTADO_SELECT_TIPO_PARTIDA:
					break;
				case ESTADO_SELECT_NIVELES:
					ev_gl.estado_juego = seleccionar_niveles(&ev_gl, &rec_menu);
					break;
				case ESTADO_JUEGO:
					ev_gl.estado_juego=juego_principal(&ev_gl, &evento);
					break;
				case ESTADO_GAMEOVER:
					break;
				case ESTADO_SALIR:
					ev_gl.corriendo = false;
					game_log(LOG_DEBUG, "ESTADO_SALIR HA SIDO RETORNADO POR UNA FUNCION", 0);
					break;
				default:
					break;
			}

	}

	if (ev_gl.mando_p1 != NULL) {
		SDL_GameControllerClose(ev_gl.mando_p1);
		ev_gl.mando_p1 = NULL;
	}
	if (ev_gl.mando_p2 != NULL) {
		SDL_GameControllerClose(ev_gl.mando_p2);
		ev_gl.mando_p2 = NULL;
	}

	if (ev_gl.mapa_actual.arreglo_notas != NULL) {
		free(ev_gl.mapa_actual.arreglo_notas);
		ev_gl.mapa_actual.arreglo_notas = NULL;
	}

	if (rec_menu.textura_titulo) SDL_DestroyTexture(rec_menu.textura_titulo);
	if (rec_menu.sprites) SDL_DestroyTexture(rec_menu.sprites);
	if (rec_menu.fuente) TTF_CloseFont(rec_menu.fuente);
	if (rec_menu.sfx_opcion) Mix_FreeChunk(rec_menu.sfx_opcion);

	Mix_HaltMusic();
	if (rec_menu.musica_fondo) Mix_FreeMusic(rec_menu.musica_fondo);
	Mix_CloseAudio();

	if (ev_gl.renderizado) SDL_DestroyRenderer(ev_gl.renderizado);
	if (ev_gl.ventana) SDL_DestroyWindow(ev_gl.ventana);

	Mix_Quit();
	TTF_Quit();
	IMG_Quit(); 
	SDL_Quit();

	return 0;
}


void iniciar_recursos_menu(menu_principal_recursos *rec_menu, eventos_globales *ev_gl)
{
	rec_menu->opcion=0;
	rec_menu->fuente = TTF_OpenFont("./assets/fonts/Silver.ttf", 64);
	rec_menu->sprites = IMG_LoadTexture(ev_gl->renderizado, "./assets/Sprite-0002.png");

	rec_menu->color1.r = 255;
	rec_menu->color1.g = 255;
	rec_menu->color1.b = 255;

	rec_menu->color2.r = 70;
	rec_menu->color2.g = 255;
	rec_menu->color2.b = 0;

	rec_menu->color3.r = 0;
	rec_menu->color3.g = 34;
	rec_menu->color3.b = 255;
	
	rec_menu->musica_fondo= Mix_LoadMUS("./assets/music/zenith_sector/bg_start_3.ogg");
	rec_menu->sfx_opcion = Mix_LoadWAV("./assets/sfx/undertale-select-sound.wav");

	if (rec_menu->fuente) 
	{
        SDL_Surface *surf = TTF_RenderText_Solid(rec_menu->fuente, "Feel, Amplify and Conquer!", rec_menu->color2);
        rec_menu->textura_titulo = SDL_CreateTextureFromSurface(ev_gl->renderizado, surf);
        rec_menu->titulo_w = surf->w;
        rec_menu->titulo_h = surf->h;
        SDL_FreeSurface(surf);
   }

	rec_menu->scroll_edificios = 0.0f;
   rec_menu->vel_edificios = 300.0f;

	if (rec_menu->musica_fondo)
		Mix_PlayMusic(rec_menu->musica_fondo, -1);
}

void iniciar_eventos_globales(eventos_globales *ev_gl)
{

	ev_gl->estado_juego=ESTADO_MENU;
	ev_gl->corriendo = true;
	ev_gl->ventana = crear_ventana();
	ev_gl->renderizado = SDL_CreateRenderer(ev_gl->ventana, -1, SDL_RENDERER_ACCELERATED);
	ev_gl->iris_radius = 800.0;
	ev_gl->is_iris_fading_out = false;
	ev_gl->mando_p1 = NULL;
	ev_gl->mando_p2 = NULL;
	ev_gl->opcion_dificultad = 0;
	ev_gl->musica_nivel_actual = NULL;
	SDL_RenderSetLogicalSize(ev_gl->renderizado, ANCHO_PANTALLA, LARGO_PANTALLA);

}

void eventos_globales_accionados_simples(eventos_globales *ev_gl, SDL_Event evento)
{
	if (evento.type == SDL_QUIT) 
	{
			game_log(LOG_INFO, "Cerrando ventana por accion del usuario a traves del servidor grafico ", 0);
			ev_gl->corriendo = false;
	}

	if (evento.type == SDL_KEYDOWN &&  evento.key.repeat == 0)
	{
		if (evento.key.keysym.sym == SDLK_F9)
			Pantalla_Completa(ev_gl->ventana);
		if (evento.key.keysym.sym == SDLK_ESCAPE)
		{
			game_log(LOG_DEBUG, "cierre por fin del loop a traves de la bandera 'corriendo', accionado por tecla ESCAPE", 0);
			ev_gl->corriendo = false;
		}
	}

}

void iniciar_componente()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) 
		game_log(LOG_ERROR, "SDL no pudo inicializarse! Error: %s\n", SDL_GetError());
	/* base de datos sdl comunitaria para dispositivos plug */
	if (SDL_GameControllerAddMappingsFromFile("./gamecontrollerdb.txt") < 0) {
		game_log(LOG_WARN, "No se encontro gamecontrollerdb.txt o hubo un error", SDL_GetError());
	}
	if (TTF_Init() == -1) 
		game_log(LOG_ERROR, "SDL_ttf error: %s\n", TTF_GetError());

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		printf("Error: %s\n", Mix_GetError());

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	game_log(LOG_DEBUG, "Componentes de SDL iniciados correctamente!\n", 0);
}

void manejo_delta_time(float *delta_time, unsigned int *last_frame_time)
{
	unsigned int current_time = SDL_GetTicks();

	*delta_time = (current_time - *last_frame_time) / 1000.0f;

	*last_frame_time = SDL_GetTicks();

	if (*delta_time > 0.1f) *delta_time = 0.1f;

	*last_frame_time = current_time;	

	unsigned int frame_duration = SDL_GetTicks() - current_time;
	if (frame_duration < FRAME_TARGET_TIME)
		SDL_Delay(FRAME_TARGET_TIME - frame_duration);
}

void eventos_accionados_usuario(eventos_globales *ev_gl, SDL_Event evento, menu_principal_recursos *rec_menu)
{
	while (SDL_PollEvent(&evento))
		{
			eventos_globales_accionados_simples(ev_gl, evento);
			manejo_mando(ev_gl, evento, rec_menu);
			if (evento.type == SDL_KEYDOWN && evento.key.repeat == 0)
			{
				if (ev_gl->estado_juego == ESTADO_MENU) 
				{
					teclas_menu_principal(rec_menu, evento);

					if (evento.key.keysym.sym == SDLK_RETURN)
					{
						if (rec_menu->musica_fondo)
							Mix_PlayMusic(rec_menu->musica_fondo, -1);

						if (rec_menu->opcion == 0)
							ev_gl->estado_juego = ESTADO_SELECT_NIVELES;
						if (rec_menu->opcion == 1)
							ev_gl->estado_juego = ESTADO_SALIR;
						if (rec_menu->opcion == 2)
							ev_gl->estado_juego = ESTADO_SALIR;
					}
				}
				else if (ev_gl->estado_juego == ESTADO_JUEGO)
        {
			    teclas_juego_principal(ev_gl, evento);
        }
				else if (ev_gl->estado_juego == ESTADO_SELECT_NIVELES)
				{
				    if (evento.key.keysym.sym == SDLK_UP && ev_gl->opcion_dificultad > 0)
				    {
				        ev_gl->opcion_dificultad--;
				        if (rec_menu->sfx_opcion) Mix_PlayChannel(-1, rec_menu->sfx_opcion, 0);
				    }
				    if (evento.key.keysym.sym == SDLK_DOWN && ev_gl->opcion_dificultad < 2)
				    {
				        ev_gl->opcion_dificultad++;
				        if (rec_menu->sfx_opcion) Mix_PlayChannel(-1, rec_menu->sfx_opcion, 0);
				    }
				    if (evento.key.keysym.sym == SDLK_ESCAPE) {
				        ev_gl->estado_juego = ESTADO_MENU; /* Volver al menú principal */
				    }
				    if (evento.key.keysym.sym == SDLK_RETURN) {
				        /* Dificultad mapea a 1, 2 o 3 */
				        generar_playlist_aleatoria(ev_gl, ev_gl->opcion_dificultad + 1);

				        if (ev_gl->playlist.modo_playlist && ev_gl->playlist.cantidad > 0) {
				            ev_gl->mapa_actual = cargar_nivel(ev_gl->playlist.rutas[0]);
				            ev_gl->tiempo_juego = 0.0f;
				            ev_gl->estado_juego = ESTADO_JUEGO;
				            game_log(LOG_INFO, "Iniciando partida aleatoria.", 0);

				            if (ev_gl->musica_nivel_actual != NULL) {
				                Mix_HaltMusic();
				                Mix_FreeMusic(ev_gl->musica_nivel_actual);
				            }
				            ev_gl->musica_nivel_actual = Mix_LoadMUS(ev_gl->playlist.rutas_audio[0]);
				            if (ev_gl->musica_nivel_actual) {
				                Mix_PlayMusic(ev_gl->musica_nivel_actual, 0); /* 0 denota cero repeticiones */
				            } else {
				                game_log(LOG_ERROR, "No se pudo cargar audio P1", Mix_GetError());
				            }

				        } else {
				            ev_gl->estado_juego = ESTADO_MENU; /* Resguardo si la carpeta está vacía */
				        }
				    }
				}
			}
		}

}

void teclas_menu_principal(menu_principal_recursos *rec_menu, SDL_Event evento)
{
	int opcion_anterior = rec_menu->opcion;

	if (evento.key.keysym.sym == SDLK_UP && rec_menu->opcion > 0)
		rec_menu->opcion--;
	if (evento.key.keysym.sym == SDLK_DOWN && rec_menu->opcion < 3)
		rec_menu->opcion++;

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



void evaluar_golpe(int carril_presionado, int jugador, eventos_globales *ev_gl)
{
    for (int i = ev_gl->mapa_actual.notas_pasadas; i < ev_gl->mapa_actual.total_notas; i++)
    {
        Nota *n = &ev_gl->mapa_actual.arreglo_notas[i];

        if (n->activa == false || n->carril != carril_presionado)
            continue;

        float delta_t = (float)fabs(n->tiempo_golpe - ev_gl->tiempo_juego);

        if (delta_t > 0.20f && n->tiempo_golpe > ev_gl->tiempo_juego)
            break;

        char msg[64];

        if (delta_t <= 0.05f) {
            snprintf(msg, sizeof(msg), "[P%d] PERFECT", jugador);
            game_log(LOG_INFO, msg, 0);
            n->activa = false; 
            break;
        } else if (delta_t <= 0.10f) {
            snprintf(msg, sizeof(msg), "[P%d] GOOD", jugador);
            game_log(LOG_INFO, msg, 0);
            n->activa = false;
            break;
        } else if (delta_t <= 0.20f) {
            snprintf(msg, sizeof(msg), "[P%d] BAD", jugador);
            game_log(LOG_INFO, msg, 0);
            n->activa = false;
            break;
        }
    }
}

void teclas_juego_principal(eventos_globales *ev_gl, SDL_Event evento)
{
	int carril_presionado = -1;
	int jugador = -1;

	/* Carriles P1 */
	if (evento.key.keysym.sym == SDLK_h)      { carril_presionado = 0; jugador = 1; }
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



void manejo_mando(eventos_globales *ev_gl,  SDL_Event evento, menu_principal_recursos *rec_menu)
{
	if (evento.type == SDL_CONTROLLERDEVICEADDED) 
	{
		SDL_GameController *mando_temporal = SDL_GameControllerOpen(evento.cdevice.which);
		if (mando_temporal) 
		{
			const char *nombre = SDL_GameControllerName(mando_temporal);

			if (nombre && strstr(nombre, "Microntek") != NULL) 
			{
				if (ev_gl->mando_p1 == NULL)
				{
				ev_gl->mando_p1 = mando_temporal;
				game_log(LOG_INFO, "Alfombra asignada a Jugador 1", nombre);
				} else {
					SDL_GameControllerClose(mando_temporal);
				}
			} 
			else 
			{
				if (ev_gl->mando_p2 == NULL)
				{
					ev_gl->mando_p2 = mando_temporal;
					game_log(LOG_INFO, "Mando convencional asignado a Jugador 2", nombre);
				} else {
					SDL_GameControllerClose(mando_temporal);
				}
			}
		}
	} 
	else if (evento.type == SDL_CONTROLLERDEVICEREMOVED) 
	{
		SDL_GameController *mando_desconectado = SDL_GameControllerFromInstanceID(evento.cdevice.which);
		if (ev_gl->mando_p1 == mando_desconectado)
		{
			SDL_GameControllerClose(ev_gl->mando_p1);
			ev_gl->mando_p1 = NULL;
			game_log(LOG_INFO, "Alfombra del Jugador 1 desconectada", 0);
		}
		else if (ev_gl->mando_p2 == mando_desconectado)
		{
				SDL_GameControllerClose(ev_gl->mando_p2);
			ev_gl->mando_p2 = NULL;
				game_log(LOG_INFO, "Mando del Jugador 2 desconectado", 0);
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


void botones_mando_juego_principal(eventos_globales *ev_gl, SDL_Event evento, menu_principal_recursos *rec_menu)
{
	int carril_presionado = -1;
	int offset_carril = -1;
	int jugador = -1;

	if (ev_gl->mando_p1 && evento.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(ev_gl->mando_p1))) {
		offset_carril = 0;
		jugador = 1;
	} 
	else if (ev_gl->mando_p2 && evento.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(ev_gl->mando_p2))) {
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

void botones_mando_menu_principal(eventos_globales *ev_gl, SDL_Event evento, menu_principal_recursos *rec_menu)
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
			if (rec_menu->musica_fondo)
				Mix_PlayMusic(rec_menu->musica_fondo, -1);

			if (rec_menu->opcion == 0)
				ev_gl->is_iris_fading_out = true;
			else if (rec_menu->opcion == 2)
				ev_gl->estado_juego = ESTADO_SALIR;
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
