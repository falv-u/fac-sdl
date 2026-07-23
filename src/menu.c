#include <math.h>
#include "SDL_events.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include "commons.h"

void fondo_menu(event_global *ev_gl, menu_principal_recursos *rec_menu);
void fade_iris_out(event_global *ev_gl);

void grid_menu_3d(event_global *ev_gl);
void pintar_gradiente_noche_menu(SDL_Renderer *renderizado);

void opciones_menu(event_global *ev_gl, menu_principal_recursos *rec_menu);

// ESTADO_ACTUAL pantalla_carga(event_global *ev_gl, menu_principal_recursos *rec_menu);

ESTADO_ACTUAL menu_principal(event_global *ev_gl, SDL_Event *evento, menu_principal_recursos *rec_menu)
{
	(void)evento; /* para eliminar warning, no relevante */

	SDL_RenderClear(ev_gl->renderizado);
	
	pintar_gradiente_noche_menu(ev_gl->renderizado);

	fondo_menu(ev_gl, rec_menu);


	if (ev_gl->is_iris_fading_out)
		fade_iris_out(ev_gl);

 	/* RECORDAR DESTRUIR TEXTURAS */
	SDL_RenderPresent(ev_gl->renderizado);
	return ESTADO_MENU;
}

void opciones_menu(event_global *ev_gl, menu_principal_recursos *rec_menu)
{
	/* ns: no_select, s: select */
	SDL_Rect src_opcion_s = {0*64, 5*64, 64*2, 64*2};
	SDL_Rect src_opcion_ns = {0*64, 6*64, 64*2, 64*2};

	SDL_Rect dest_opcion_s = {ANCHO_PANTALLA/6, LARGO_PANTALLA/2.5, 64*(ESCALADO_1+0.6), 64*1.5};
	SDL_Rect dest_opcion_ns = {ANCHO_PANTALLA/12, LARGO_PANTALLA/2.5, 64*(ESCALADO_1+0.6), 64*1.5};
	SDL_Rect dest_opcion_ns_1 = {ANCHO_PANTALLA/12, LARGO_PANTALLA/1.8, 64*(ESCALADO_1+0.6), 64*1.5};
	SDL_Rect dest_opcion_ns_2 = {ANCHO_PANTALLA/12, LARGO_PANTALLA/1.4, 64*(ESCALADO_1+0.6), 64*1.5};

	SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_opcion_ns, &dest_opcion_ns);
	SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_opcion_ns, &dest_opcion_ns_1);
	SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_opcion_ns, &dest_opcion_ns_2);
	SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_opcion_s, &dest_opcion_s);
}
void fondo_menu(event_global *ev_gl, menu_principal_recursos *rec_menu)
{
		grid_menu_3d(ev_gl);

    if (rec_menu->sprites) 
    {
        SDL_Rect src_moon = {14*64, 0*64, 64*2, 64*2};
        
       /* Centramos la luna */
       int x_luna = (ANCHO_PANTALLA / 2) - 64;
       SDL_Rect dest_moon = {x_luna, 60, 64*2, 64*2};

       /* Aplicar color magenta y dibujar el resplandor, aumenta el radio */
       SDL_SetTextureColorMod(rec_menu->sprites, 255, 0, 255);
       /* NOTA: cuidado con el bloom, puede sobrecargar cpu segun me dicen */
       SDL_SetTextureBlendMode(rec_menu->sprites, SDL_BLENDMODE_ADD);
       SDL_SetTextureAlphaMod(rec_menu->sprites, 100);
       SDL_Rect dest_moon_bloom = { dest_moon.x - 15, dest_moon.y - 15, dest_moon.w + 30, dest_moon.h + 30 };
       SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_moon, &dest_moon_bloom);

			 /* filtros magicos sacados desde internet... */
       SDL_SetTextureBlendMode(rec_menu->sprites, SDL_BLENDMODE_BLEND);
       SDL_SetTextureAlphaMod(rec_menu->sprites, 255);
       SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, &src_moon, &dest_moon);
       SDL_SetTextureColorMod(rec_menu->sprites, 255, 255, 255);

       int ancho_final_opt = (int)(64 * 3.0); /* Botones más anchos */
       int alto_final_opt  = (int)(64 * 1.2);
        
       int x_botones = (ANCHO_PANTALLA / 2) - (ancho_final_opt / 2);
       int y_inicial_opt = (int)(LARGO_PANTALLA / 2.2);
       int espaciado_opt = alto_final_opt + 20;

       SDL_Rect src_opcion_ns = {0, 4*64, 64*2, 64}; 
       SDL_Rect src_opcion_s  = {0, 5*64, 64*2, 64}; 

       SDL_Rect dest_opcion1 = {x_botones, y_inicial_opt, ancho_final_opt, alto_final_opt};
       SDL_Rect dest_opcion2 = {x_botones, y_inicial_opt + espaciado_opt, ancho_final_opt, alto_final_opt};
       SDL_Rect dest_opcion3 = {x_botones, y_inicial_opt + (espaciado_opt * 2), ancho_final_opt, alto_final_opt};

       SDL_Rect *src_opt_1 = (rec_menu->opcion == 0) ? &src_opcion_s : &src_opcion_ns;
       SDL_Rect *src_opt_2 = (rec_menu->opcion == 1) ? &src_opcion_s : &src_opcion_ns;
       SDL_Rect *src_opt_3 = (rec_menu->opcion == 2) ? &src_opcion_s : &src_opcion_ns;

       SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, src_opt_1, &dest_opcion1);
       SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, src_opt_2, &dest_opcion2);
       SDL_RenderCopy(ev_gl->renderizado, rec_menu->sprites, src_opt_3, &dest_opcion3);

       /* aplicando fuerza bruta para no dibujar de vuelta todas las opts */
       /* NOTA: solucion temporal, remplzar a posteriori */
       if (rec_menu->fuente) 
       {
           SDL_Color color_blanco = {255, 255, 255, 255};
           char* textos_botones[3] = {"Jugar", "Salir", "Ranking"};
           SDL_Rect* destinos_botones[3] = {&dest_opcion1, &dest_opcion2, &dest_opcion3};

           for (int i = 0; i < 3; i++) 
           {
               SDL_Surface *surf = TTF_RenderText_Solid(rec_menu->fuente, textos_botones[i], color_blanco);
               if (surf) 
               {
                   SDL_Texture *tex = SDL_CreateTextureFromSurface(ev_gl->renderizado, surf);
                    
                   SDL_Rect dest_texto = {
                       destinos_botones[i]->x + (destinos_botones[i]->w - surf->w) / 2,
                       destinos_botones[i]->y + (destinos_botones[i]->h - surf->h) / 2,
                       surf->w,
                       surf->h
                   };
                   SDL_RenderCopy(ev_gl->renderizado, tex, NULL, &dest_texto);
                   
                   SDL_FreeSurface(surf);
                   SDL_DestroyTexture(tex);
               }
           }
       }
   }

   SDL_Rect dest_titulo = { 100, 100, rec_menu->titulo_w, rec_menu->titulo_h };
   if (rec_menu->textura_titulo) {
       SDL_RenderCopy(ev_gl->renderizado, rec_menu->textura_titulo, NULL, &dest_titulo);
   }
}

void grid_menu_3d(event_global *ev_gl)
{
    SDL_SetRenderDrawBlendMode(ev_gl->renderizado, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ev_gl->renderizado, 255, 0, 255, 100); /* Magenta translúcido */

    int horizonte_y = LARGO_PANTALLA / 2;
    int centro_x = ANCHO_PANTALLA / 2;
    float velocidad = 20.0f;
    float espacio_z = 10.0f;
    float z_max = 200.0f;
    
    float offset_z = fmodf(ev_gl->tiempo_juego * velocidad, espacio_z);

    /* ineas horizontales con deformación */
   for (float z = 1.0f; z < z_max; z += espacio_z)
   {
       float z_actual = z - offset_z;
       if (z_actual <= 0.1f) continue;

       int y_pantalla = horizonte_y + (int)(800.0f / z_actual);

       if (y_pantalla < LARGO_PANTALLA && y_pantalla >= horizonte_y) {
           int x_previo = 0;
           /* La altura base modificada por un sin() para crear la primera onda */
           int y_previo = y_pantalla + (int)(sin(ev_gl->tiempo_juego) * (60.0f / z_actual));

           /* dibujamos la linea por segmentos de 20 píxeles para poder curvarla */
           for (int x = 20; x <= ANCHO_PANTALLA; x += 20)
            {
                int y_ondulado = y_pantalla + (int)(sin(x * 0.05f + ev_gl->tiempo_juego) * (60.0f / z_actual));
                SDL_RenderDrawLine(ev_gl->renderizado, x_previo, y_previo, x, y_ondulado);
                x_previo = x;
                y_previo = y_ondulado;
            }
        }
    }

    for (int i = -15; i <= 15; i++)
    {
        int x_fondo = centro_x + (i * 150);
        SDL_RenderDrawLine(ev_gl->renderizado, centro_x, horizonte_y, x_fondo, LARGO_PANTALLA);
    }

    SDL_SetRenderDrawBlendMode(ev_gl->renderizado, SDL_BLENDMODE_NONE);
}

void fade_iris_out(event_global *ev_gl)
{
	SDL_SetRenderDrawColor(ev_gl->renderizado, 0, 0, 0, 255);

	int w = ANCHO_PANTALLA;
	int h = LARGO_PANTALLA;
	float r = ev_gl->iris_radius;
	int cx = w / 2;
	int cy = h / 2;

	SDL_Rect top    = {0, 0, w, cy - (int)r};
	SDL_Rect bottom = {0, cy + (int)r, w, h - (cy + (int)r)};
	SDL_Rect left   = {0, cy - (int)r, cx - (int)r, (int)r * 2};
	SDL_Rect right  = {cx + (int)r, cy - (int)r, w - (cx + (int)r), (int)r * 2};

	SDL_RenderFillRect(ev_gl->renderizado, &top);
	SDL_RenderFillRect(ev_gl->renderizado, &bottom);
	SDL_RenderFillRect(ev_gl->renderizado, &left);
	SDL_RenderFillRect(ev_gl->renderizado, &right);
}

void pintar_gradiente_noche_menu(SDL_Renderer *render)
{
	SDL_Color color_cenit = {5, 10, 35, 255};   /* Azul noche superior */
	SDL_Color color_horizonte = {0, 0, 5, 255}; /* horizonte inferior */

	SDL_Vertex vertices[4];

	/* arriba IIzquierda */
	vertices[0].position.x = 0.0f;
	vertices[0].position.y = 0.0f;
	vertices[0].color = color_cenit;
	vertices[0].tex_coord.x = 0.0f;
	vertices[0].tex_coord.y = 0.0f;

	/* Arriba Derecha */
	vertices[1].position.x = (float)ANCHO_PANTALLA;
	vertices[1].position.y = 0.0f;
	vertices[1].color = color_cenit;
	vertices[1].tex_coord.x = 1.0f;
	vertices[1].tex_coord.y = 0.0f;

	/* Abajo Derecha */
	vertices[2].position.x = (float)ANCHO_PANTALLA;
	vertices[2].position.y = (float)LARGO_PANTALLA;
	vertices[2].color = color_horizonte;
	vertices[2].tex_coord.x = 1.0f;
	vertices[2].tex_coord.y = 1.0f;

	/* Abajo Izquierda */
	vertices[3].position.x = 0.0f;
	vertices[3].position.y = (float)LARGO_PANTALLA;
	vertices[3].color = color_horizonte;
	vertices[3].tex_coord.x = 0.0f;
	vertices[3].tex_coord.y = 1.0f;

	int indices[6] = {0, 1, 2, 0, 2, 3};

	/* Renderizado sin textura para usar solo color */
	SDL_RenderGeometry(render, NULL, vertices, 4, indices, 6);
}

/* NOTA: sin implementar, propensa a fallos, arreglar lo siguiente */
/* TODO:- solapa la musica
        - No renderiza bien sobre el fondo
        - se come la cpu (es decir, error de logica en alguna parte)
        - No tengo portada aun asi que basicamente se vuelve loca al no encontrarla
        - otros bugs menores
*/
ESTADO_ACTUAL pantalla_carga(event_global *ev_gl, menu_principal_recursos *rec_menu)
{
    /* Las variables estaticas mantienen su valor entre los llamados de funcion...o eso dice el manual */
    static bool inicializado = false;
    static SDL_Texture *tex_portada = NULL;
    static SDL_Texture *tex_tit = NULL;
    static SDL_Texture *tex_dur = NULL;
    static unsigned int tiempo_inicio = 0;

    if (!inicializado) {
        Mix_HaltMusic(); /* Detener mus del menu */

        int idx = ev_gl->playlist.actual;
        char *ruta_portada = ev_gl->playlist.portadas[idx];
        char *titulo = ev_gl->playlist.titulos[idx];
        float duracion = ev_gl->playlist.duraciones[idx];

        /* intentar cargar portada */
        tex_portada = IMG_LoadTexture(ev_gl->renderizado, ruta_portada);

        if (rec_menu->fuente) {
            SDL_Color blanco = {255, 255, 255, 255};
            char buffer_duracion[32];
            snprintf(buffer_duracion, sizeof(buffer_duracion), "Duracion: %d:%02d", (int)(duracion / 60), (int)duracion % 60);

            /* renderiza */
            SDL_Surface *surf_tit = TTF_RenderText_Solid(rec_menu->fuente, titulo, blanco);
            SDL_Surface *surf_dur = TTF_RenderText_Solid(rec_menu->fuente, buffer_duracion, blanco);

            /* crea textura y libera memoria NO OLVIDAR EL FREE */
            if (surf_tit) {
                tex_tit = SDL_CreateTextureFromSurface(ev_gl->renderizado, surf_tit);
                SDL_FreeSurface(surf_tit);
            }
            if (surf_dur) {
                tex_dur = SDL_CreateTextureFromSurface(ev_gl->renderizado, surf_dur);
                SDL_FreeSurface(surf_dur);
            }
        }

        tiempo_inicio = SDL_GetTicks();
        inicializado = true;
    }

    SDL_SetRenderDrawColor(ev_gl->renderizado, 15, 15, 20, 255); /* color arbitrario btw */
    SDL_RenderClear(ev_gl->renderizado);

    SDL_Rect dest_portada = { (ANCHO_PANTALLA / 2) - 150, (LARGO_PANTALLA / 2) - 200, 300, 300 };
    
    if (tex_portada)
    {
        SDL_RenderCopy(ev_gl->renderizado, tex_portada, NULL, &dest_portada);
    } else {
        SDL_SetRenderDrawColor(ev_gl->renderizado, 50, 50, 50, 255);
        SDL_RenderFillRect(ev_gl->renderizado, &dest_portada); 
    }

    if (tex_tit)
    {
        int w, h;
        SDL_QueryTexture(tex_tit, NULL, NULL, &w, &h);
        SDL_Rect rect_tit = { (ANCHO_PANTALLA / 2) - (w / 2), (LARGO_PANTALLA / 2) + 120, w, h };
        SDL_RenderCopy(ev_gl->renderizado, tex_tit, NULL, &rect_tit);
    }
    if (tex_dur)
    {
        int w, h;
        SDL_QueryTexture(tex_dur, NULL, NULL, &w, &h);
        SDL_Rect rect_dur = { (ANCHO_PANTALLA / 2) - (w / 2), (LARGO_PANTALLA / 2) + 180, w, h };
        SDL_RenderCopy(ev_gl->renderizado, tex_dur, NULL, &rect_dur);
    }

    SDL_RenderPresent(ev_gl->renderizado);

    /* NOTA: 2 segundos*/
    if (SDL_GetTicks() - tiempo_inicio > 2000)
    {
        if (tex_portada) SDL_DestroyTexture(tex_portada);
        if (tex_tit) SDL_DestroyTexture(tex_tit);
        if (tex_dur) SDL_DestroyTexture(tex_dur);
        
        inicializado = false; /* Resetear */
        
        // if (ev_gl->musica_nivel_actual)
        //     Mix_PlayMusic(ev_gl->musica_nivel_actual, 0); 
        
        
        return ESTADO_JUEGO;
    }

    return ESTADO_CARGA;
}
