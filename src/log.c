#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "log.h"
#include "commons.h"

static const nivel_log NIVEL_MINIMO = LOG_LEVEL;
void tiempo_actual();

void game_log(nivel_log level, const char *mensaje, const char *mensaje_SDL, ...)
{

	if (level < NIVEL_MINIMO) {
		return;
	}
	switch (level) {
	case LOG_DEBUG:
		// Verde
		printf("\033[0;32m[DEBUG]\033[0m %s\n", mensaje);
		break;

	case LOG_INFO:
		// normal
		printf("[INFO] %s\n", mensaje);
		break;

	case LOG_WARN:
		// Amarillo
		printf("\033[0;33m[WARN]\033[0m %s\n", mensaje);
		break;

	case LOG_ERROR:
		// Rojo y dirigido a stderr (salida de errores)
		fprintf(stderr, "\033[0;31m[ERROR]\033[0m %s\n", mensaje);
		break;
	}
}

void tiempo_actual()
{
    time_t tiempo_actual;
    
    time(&tiempo_actual);
}
