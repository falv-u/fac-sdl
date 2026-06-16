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
	const char *carpeta_niveles = "/home/frani/code/fac-sdl/niveles/";
	dir = opendir(carpeta_niveles);

	if (dir == NULL)
	{
		game_log(LOG_ERROR, "CARPETA NO ENCONTRADA!", NULL);
		return;
	}

	printf("%-25s %-10s %-20s\n", "NOMBRE", "TAM (Bytes)", "MODIFICADO");
   printf("-------------------------------------------------------------\n");

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name,  "..") == 0) 
			continue;

		snprintf(ruta_completa, sizeof(ruta_completa), "%s%s", carpeta_niveles, entry->d_name);

		if (stat(ruta_completa, &file_stat) == 0)
      {
         strftime(time_srt, sizeof(time_srt), "%d-%m-%Y %H:%M", localtime(&file_stat.st_mtime));
         printf("%-25s %-10ld %-20s\n", entry->d_name, (long)file_stat.st_size, time_srt);
        }else
        {
            printf("%-25s %-10s %-20s\n", entry->d_name, "ERROR", "---");
        }
	}
	closedir(dir);
}
