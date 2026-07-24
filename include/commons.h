#ifndef COMMONS_H
#define COMMONS_H
/* standar headers */
#include <stdbool.h>

/* SDL libs */
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL_gamecontroller.h>
#include <SDL_mixer.h>
#include <SDL_rect.h>
#include <SDL_video.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_audio.h>

/* Libs propias */
/*(ninguna por ahora)*/

/* Macros */

/* log.h */
#define LOG_LEVEL LOG_DEBUG /* Maquina de estado */

/* Jugabilidad */
#define MAX_CANCIONES_PLAYLIST 4
#define MAX_PLAYERS 2
#define Y_JUEZ 600.0f
#define Y_SPAWN -64.0f
#define TIEMPO_APROXIMACION 1.5f /* La flecha tarda 1.5s en caer desde que aparece */

/* Delta Time */
#define FPS 30
#define TARGET_FPS      60
#define FRAME_TARGET_TIME (1000/FPS)



/* renderizado */
#define LARGO_PANTALLA 720
#define ANCHO_PANTALLA 1280
#define ESCALADO_1 3

#define TILE_64 64

extern int last_frame_time;



/* MENU */
typedef struct {
    int src_x, src_y;  // Coordenadas de origen 1024x1024
    int dest_x, dest_y;// Coordenadas de destino en la pantalla
    int ancho, alto;   // Tamaño del bloque  64x64
    int escala;        // Factor de escalado
} ElementosMenu;


typedef struct {
	/* menu_principal */
	SDL_Rect jugar;
	SDL_Rect opciones;
	SDL_Rect salir;

	SDL_Texture* textura_titulo;
	int titulo_w, titulo_h;
	SDL_Texture *sprites;

	float scroll_camino;
	float scroll_edificios;

	float vel_camino;

	float vel_edificios;
	SDL_Color color1;
	SDL_Color color2;
	SDL_Color color3;
	
	Mix_Music *musica_fondo;
	Mix_Chunk *sfx_opcion;
	int opcion;

} menu_recursos;

/*--------------------------------------------------------------*/
/* Logica de juego */
typedef enum {
	NOTA_SIMPLE,
	NOTA_LARGA
} TIPO_NOTA;

typedef enum {
	NOTA_ESPERANDO,   /* sigue bajando */
	NOTA_MANTENIDA,   /* el jugador continua presionando */
	NOTA_GOLPEADA,    /* acertada */
	NOTA_FALLADA      /* pasa de largo */
} ESTADO_NOTA;


typedef struct {
	float tiempo_golpe;  /* l momento exacto en que la base de la nota toca al juez */
	float duracion;      /* si es 0.0f, es nota normal. Si es > 0.0f, es tecla larga */
	int carril;          /* 0 a 3 (Jugador 1) | 4 a 7 (Jugador 2) */
	float y_actual;      /* Posición calculada de la base */
	bool activa;
	bool siendo_presionada; /* TRUE si el jugador la está manteniendo pisada */
} Nota;

typedef struct {
	SDL_Keycode teclas_p1[4];
	SDL_Keycode teclas_p2[4];
	bool teclas_aleatorias; /* Bandera que lee el parser del nivel.txt */
	Nota *arreglo_notas;
	int total_notas;
	int notas_pasadas;
} Mapa;

typedef struct {
	bool single_player;
	bool niveles_aleatorios;
	int dificultad;
} tipo_part;

/* ---------------------------------------------- */

/* eventos y estados globables (requeridos por todo el juego) */
typedef enum {
	ESTADO_MENU,
	ESTADO_PAUSA,
	ESTADO_CARGA,
	ESTADO_SELECT_TIPO_PARTIDA,
	ESTADO_SELECT_NIVELES,
	ESTADO_JUEGO,		ESTADO_GAMEOVER,
	ESTADO_INGRESAR_NOMBRE,
	ESTADO_RANKING,
	ESTADO_DETALLE_JUGADOR,
	ESTADO_SALIR,
} ESTADO_ACTUAL;

typedef struct Play_D{
    char rutas[MAX_CANCIONES_PLAYLIST][512];
    char titulos[MAX_CANCIONES_PLAYLIST][256];      
    float duraciones[MAX_CANCIONES_PLAYLIST];        
    char portadas[MAX_CANCIONES_PLAYLIST][512];
    char rutas_audio[MAX_CANCIONES_PLAYLIST][512];
    int cantidad;          
    int actual;            
    bool modo_playlist;    
} Play_D;

typedef struct jugador {
	SDL_GameController *mando;
	int puntaje;
	int perfect, good, bad, miss;
	float barra_ki;
	float multiplicador;
	bool bono_50_dado;
	int bono_acumulado;
	float tiempo_x3_restante;
	float tiempo_jugado;

	/* Efectos visuales de la pared */
	float intensidad_pared;
	SDL_Color color_pared;
} jugador;

typedef struct visuales {
	SDL_Renderer	*renderizado;
	SDL_Window	*ventana;
	TTF_Font	*fuente[2];
	float iris_radius;
	bool		is_iris_fading_out;
} visuales;

typedef struct partida {
	
	float tiempo_juego;
	Mapa		mapa_actual;
	Mix_Music	*musica_nivel_actual;
	jugador		jugadores[2];
	int opcion_dificultad;

	int indice_nota_actual;
        bool		pausado;
	bool		musica_iniciada;
} partida;

typedef struct sistema {
	Play_D		playlist;

	char nombre_ingresado[32];
        bool		corriendo;

	int jugador_pidiendo_nombre;

	/* fila seleccionada en la pantalla de ranking */
	int ranking_cursor;         
	/* que fila se esta mostrando en detalle */
	int ranking_detalle_indice;
	int last_frame_time;
} sistema;

typedef struct event_global {
	visuales	vis;
	partida		par;
	sistema		sis;
	menu_recursos	*rec_menu;
	ESTADO_ACTUAL	estado_juego;
} event_global;

typedef enum {
    VISUAL_PERFECT,
    VISUAL_GOOD,
    VISUAL_MISS
} TipoImpacto;

typedef struct {
    float intensidad;    // Va de 1.0f a 0.0f (apagado)
    SDL_Color color;     
} pared_ecualizador;

/* otros, para efectos visuales y demas */
typedef struct {
	  pared_ecualizador p2; // Pared derecha (Jug	dor 2)
    pared_ecualizador p1; // Pared izquierda		(Jugador 1)
} sistem_ecualizador;
/* -------------------------------------------------- */
/* primitivas */

/* renderizado en pantalla */

SDL_Window	*crear_ventana();
void		 Pantalla_Completa(SDL_Window *ventana);
void		 update(float delta_time, event_global *ev_gl, menu_recursos *rec_menu);
void		 assets_load(void);
void		 activar_efecto_pared(event_global *ev_gl, int jugador, int resultado); 

/* primitivas de ESTADO_JUEGO*/
ESTADO_ACTUAL menu_principal(event_global *ev_gl, SDL_Event *evento, menu_recursos *rec_menu);
ESTADO_ACTUAL pantalla_carga(event_global *ev_gl, menu_recursos *rec_menu);
ESTADO_ACTUAL seleccionar_niveles(event_global *ev_gl, menu_recursos *rec_menu);
void generar_playlist_aleatoria(event_global *ev_gl, int dificultad_deseada);
ESTADO_ACTUAL juego_principal(event_global *ev_gl, SDL_Event *evento, TTF_Font *fuente);

/* primitivas de selector de niveles */
Mapa cargar_nivel(const char *ruta_archivo);
Mapa cargar_nivel_desde_lista(int indice_elegido);

/* primitivas de ranking / fin de partida */
ESTADO_ACTUAL pantalla_ranking(event_global *ev_gl, SDL_Event *evento, menu_recursos *rec_menu);
ESTADO_ACTUAL pantalla_detalle_jugador(event_global *ev_gl, SDL_Event *evento, menu_recursos *rec_menu);
ESTADO_ACTUAL pantalla_ingresar_nombre(event_global *ev_gl, SDL_Event *evento, menu_recursos *rec_menu);

#endif
