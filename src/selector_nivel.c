#include "commons.h"
#include "log.h"
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <time.h>

void listar_niveles();
ESTADO_ACTUAL seleccionar_niveles()
{
	int nivel=0;
	listar_niveles();
	return ESTADO_SALIR;
}

void listar_niveles()
{
	DIR *dir;
	struct dirent *entry;
	struct stat file_stat;
	char ruta_completa[512];
	char time_srt[64];
	int total_files =0;
	FILE *archivo_salida;
	const char *carpeta_niveles = "/home/frani/code/fac-sdl/niveles/";
	const char *ruta_archivo_salida = "/home/frani/code/fac-sdl/niveles_db.cvs";

	int numero_nivel = -1;

	dir = opendir(carpeta_niveles);
	archivo_salida= fopen(ruta_archivo_salida, "w");

	if (archivo_salida == NULL)
	{
		game_log(LOG_ERROR, "NO SE PUDO CREAR EL ARCHIVO EN %s", ruta_archivo_salida);
		return;
	}

	if (dir == NULL)
	{
		game_log(LOG_ERROR, "CARPETA NO ENCONTRADA!", NULL);
		fclose(archivo_salida); // <-- CORRECCIÓN OBLIGATORIA
		return;
	}

	fprintf(archivo_salida, "Numero_Nivel;Ruta_Completa;Fecha_Modificacion\n");

	printf("\n%-7s %-50s %-12s %-16s\n", "NIVEL", "RUTA COMPLETA", "TAM (Bytes)", "MODIFICADO");
   printf("-------------------------------------------------------------\n");

	while ((entry = readdir(dir)) != NULL)
	{
		numero_nivel = -1;
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name,  "..") == 0) 
			continue;

		// Si el archivo no coincide con el patrón "nivel[NÚMERO]", sscanf devuelve 0 y se ignora.
		if (sscanf(entry->d_name, "nivel%d", &numero_nivel) == 1)
		{
			snprintf(ruta_completa, sizeof(ruta_completa), "%s%s", carpeta_niveles, entry->d_name);

			if (stat(ruta_completa, &file_stat) == 0)
				{
				// Traducir la fecha de modificación
				strftime(time_srt, sizeof(time_srt), "%d-%m-%Y %H:%M", localtime(&file_stat.st_mtime));

				// Escribir en el CSV: Número y Ruta por separado, seguidos de los datos físicos
				fprintf(archivo_salida, "%d;%s;%s\n", numero_nivel, ruta_completa, time_srt);

				printf("%-7d %-50s %-12ld %-16s\n", numero_nivel, ruta_completa, (long)file_stat.st_size, time_srt);

			}
      }
    }

    // 3. Cerrar descriptores de forma segura
    closedir(dir);
    fclose(archivo_salida); 
    
    game_log(LOG_INFO, "Datos de niveles exportados correctamente.", 0);
}

