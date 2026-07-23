#include <stdio.h>
#include <stdarg.h>

#include "log.h"
#include "commons.h"

static const nivel_log NIVEL_MINIMO = LOG_LEVEL;

void
game_log(nivel_log level, const char *mensaje, ...)
{
	va_list	ls_argumentos;

	if (level < NIVEL_MINIMO)
		return;
		
	switch (level) {
	case LOG_DEBUG:
		// Verde
		printf("\033[0;32m[DEBUG]\033[0m");
		break;

	case LOG_INFO:
		// normal
		printf("[INFO]");
		break;

	case LOG_WARN:
		// Amarillo
		printf("\033[0;33m[WARN]\033[0m");
		break;

	case LOG_ERROR:
		// Rojo y dirigido a stderr (salida de errores)
		fprintf(stderr, "\033[0;31m[ERROR]\033[0m ");
		break;
	}

	va_start(ls_argumentos, mensaje);

	vfprintf(stdout, mensaje, ls_argumentos);

	va_end(ls_argumentos);
	printf("\n");
}
