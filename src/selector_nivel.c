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

MapaCancion cargar_nivel(const char *ruta_archivo)
{
    MapaCancion mapa = {0};
    if (strlen(ruta_archivo) == 0) return mapa;

    FILE *archivo = fopen(ruta_archivo, "r");
    char linea[128];

    if (!archivo) {
        game_log(LOG_ERROR, "No se pudo abrir el mapa", ruta_archivo);
        return mapa;
    }

    while (fgets(linea, sizeof(linea), archivo))
	 {
        if (linea[0] != '#' && linea[0] != '\n') mapa.total_notas++;
    }

    mapa.arreglo_notas = (Nota *)malloc(sizeof(Nota) * mapa.total_notas);
    rewind(archivo);

    int indice = 0;
    while (fgets(linea, sizeof(linea), archivo)) 
    {
        if (linea[0] == '#' || linea[0] == '\n') continue;

        float t_golpe, duracion;
        int carril;
        
        /*  tiempo;duracion;carril */
        if (sscanf(linea, "%f;%f;%d", &t_golpe, &duracion, &carril) == 3) 
        {
            if (duracion > 0.0f) 
            {
                int largas_activas = 0;
                for (int j = 0; j < indice; j++) {
                    if (mapa.arreglo_notas[j].duracion > 0.0f) {
                        float fin_previa = mapa.arreglo_notas[j].tiempo_golpe + mapa.arreglo_notas[j].duracion;
                        if (t_golpe < fin_previa) largas_activas++; /* Hay choque temporal */
                    }
                }
                
                if (largas_activas >= 2) {
                    game_log(LOG_WARN, "Superposicion detectada. Forzando tecla corta.", linea);
                    duracion = 0.0f;
                }
            }

            /* Aleatoriedad de Carril (-1 en el txt) */
            if (carril == -1) 
            {
                int candidato;
                while (1) {
						 /* funcion rara encontrada en foro, estudiar mas tarde */
                    candidato = arc4random_uniform(8); /* 0 a 7,  */
                    int choca = 0;
                    
                    /* Revisar hacia atrás si el carril candidato está ocupado */
                    for (int j = 0; j < indice; j++) {
                        float fin_previa = mapa.arreglo_notas[j].tiempo_golpe + mapa.arreglo_notas[j].duracion;
                        if (t_golpe < fin_previa && mapa.arreglo_notas[j].carril == candidato) {
                            choca = 1; /* Carril ocupado, hay que buscar otro */
                            break;
                        }
                    }
                    if (!choca) break; 
                }
                carril = candidato;
            }

            /* Asignar a la RAM */
            mapa.arreglo_notas[indice].tiempo_golpe = t_golpe;
            mapa.arreglo_notas[indice].duracion = duracion;
            mapa.arreglo_notas[indice].carril = carril;
            mapa.arreglo_notas[indice].activa = true;
            mapa.arreglo_notas[indice].y_actual = -100.0f;
            indice++;
        }
    }

    fclose(archivo);
    return mapa;
}

MapaCancion cargar_nivel_desde_lista(int indice_elegido)
{
    listar_niveles();

    char ruta_elegida[512] = "";
    FILE *db = fopen("./niveles_db.txt", "r");
    
    if (db) {
        char linea[512];
        fgets(linea, sizeof(linea), db); /* Ignorar la cabecera que imprime tu ls */
        
        int actual = 0;
        while(fgets(linea, sizeof(linea), db)) {
            if (actual == indice_elegido) {
                int num;
                sscanf(linea, "%d;%[^;];", &num, ruta_elegida);
                break;
            }
            actual++;
        }
        fclose(db);
    }

    return cargar_nivel(ruta_elegida);
}
