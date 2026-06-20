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
	int total_files = 0;
	FILE *archivo_salida;
	const char *carpeta_niveles = "./niveles/";
	const char *ruta_archivo_salida = "./niveles_db.txt";

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
		fclose(archivo_salida); 
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

		if (sscanf(entry->d_name, "nivel%d", &numero_nivel) == 1)
		{
			snprintf(ruta_completa, sizeof(ruta_completa), "%s%s", carpeta_niveles, entry->d_name);

			if (stat(ruta_completa, &file_stat) == 0)
			{
				strftime(time_srt, sizeof(time_srt), "%d-%m-%Y %H:%M", localtime(&file_stat.st_mtime));

				fprintf(archivo_salida, "%d;%s;%s\n", numero_nivel, ruta_completa, time_srt);

				printf("%-7d %-50s %-12ld %-16s\n", numero_nivel, ruta_completa, (long)file_stat.st_size, time_srt);

			}
      }

		total_files++;
    }

    closedir(dir);
    fclose(archivo_salida); 
    
    game_log(LOG_INFO, "Datos de niveles exportados correctamente.", 0);
}

