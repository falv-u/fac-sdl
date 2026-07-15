#ifndef RANKING_H
#define RANKING_H

#include "commons.h"

#define MAX_RANKING 200
#define ARCHIVO_RANKING "./ranking.txt"

/* umbrales de la barra de ki,o XRAY, o sea el juego que venga a la mente con esta barra,
el punto es que hice una barra XD*/
#define KI_UMBRAL_BONO   50.0f
#define KI_UMBRAL_MULT   75.0f
#define KI_MAX           100.0f /* NOTA: tratarlo como % es mas facil */
#define KI_DURACION_X3   5.0f /* segundos que dura el x3 al llenarse la barra */

typedef enum {
	RESULTADO_PERFECT,
	RESULTADO_GOOD,
	RESULTADO_BAD,
	RESULTADO_MISS
} RESULTADO_GOLPE;

typedef struct {
	char nombre[32];
	int puntaje;
	int perfect, good, bad, miss;
	float tiempo_jugado;
} EntradaRanking;

/* tabla de ranking en memoria, cargada/guardada desde ranking.txt */
extern EntradaRanking tabla_ranking[MAX_RANKING];
extern int cantidad_ranking;

void ranking_cargar(void);
void ranking_guardar(void);
void ranking_agregar(const char *nombre, int puntaje, int perfect, int good, int bad, int miss, float tiempo_jugado);
void ranking_ordenar(void); /* bubble sort de mayor a menor puntaje, 90% seguro que se llamaba bubble sort */

void reiniciar_puntajes(eventos_globales *ev_gl);
void registrar_resultado(eventos_globales *ev_gl, int jugador, RESULTADO_GOLPE tipo);
void actualizar_ki_y_tiempo(eventos_globales *ev_gl, float dt);
void iniciar_flujo_fin_partida(eventos_globales *ev_gl);

#endif
