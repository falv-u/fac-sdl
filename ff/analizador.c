#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#include "kiss_fftr.h"

#define TAMANO_FRAME 1024
#define HISTORIAL_MAX 43 

#define COOLDOWN_FRAMES 4 
float calcular_magnitud(kiss_fft_cpx c);


int main(int argc, char **argv)
{
	if (argc < 2) {
	fprintf(stderr, "Uso: %s <archivo.ogg> [multiplicador_umbral] [energia_minima]\n", argv[0]);
	fprintf(stderr, "Ejemplo: %s cancion.ogg 1.8 0.5 > niveles/nivel1.txt\n", argv[0]);
	return 1;
	}

	float multiplicador_umbral = (argc >= 3) ? atof(argv[2]) : 1.8f;
	float energia_minima = (argc >= 4) ? atof(argv[3]) : 0.5f;

	int canales, frecuencia_muestreo;
	short *audio_pcm;

	int total_muestras = stb_vorbis_decode_filename(argv[1], &canales, &frecuencia_muestreo, &audio_pcm);

	if (total_muestras < 0) {
	fprintf(stderr, "Error al decodificar el archivo OGG.\n");
	return 1;
	}

	int muestras_por_canal = total_muestras;
	float *audio_mono = malloc(muestras_por_canal * sizeof(float));
	for (int i = 0; i < muestras_por_canal; i++) {
	/* Se mezcla a mono sumando canales si es necesario, aquí tomamos el L como base simplificada */
	audio_mono[i] = (float)audio_pcm[i * canales] / 32768.0f; 
	}
	free(audio_pcm);

	kiss_fftr_cfg cfg = kiss_fftr_alloc(TAMANO_FRAME, 0, NULL, NULL);
	kiss_fft_scalar *frame_entrada = malloc(TAMANO_FRAME * sizeof(kiss_fft_scalar));
	kiss_fft_cpx *frame_salida = malloc((TAMANO_FRAME / 2 + 1) * sizeof(kiss_fft_cpx));

	int total_frames = muestras_por_canal / TAMANO_FRAME;
	float historial_energia[HISTORIAL_MAX] = {0};
	int indice_historial = 0;
	int cooldown_actual = 0;

	printf("# Analizando %s (%d Hz)\n", argv[1], frecuencia_muestreo);
	printf("# Umbral: %.2fx | Energia Minima: %.2f\n", multiplicador_umbral, energia_minima);
	printf("# tiempo;duracion;carril\n");

	for (int f = 0; f < total_frames; f++)
	{

		for (int i = 0; i < TAMANO_FRAME; i++)
		{
			float muestra = audio_mono[(f * TAMANO_FRAME) + i];
			float multiplicador_ventana = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (TAMANO_FRAME - 1)));
			frame_entrada[i] = muestra * multiplicador_ventana;
		}

		kiss_fftr(cfg, frame_entrada, frame_salida);

		float energia_actual = 0.0f;
		for (int b = 1; b < 10; b++)
		{
			energia_actual += calcular_magnitud(frame_salida[b]);
		}

		float energia_promedio = 0.0f;
		for (int i = 0; i < HISTORIAL_MAX; i++)
		{
			energia_promedio += historial_energia[i];
		}
		energia_promedio /= HISTORIAL_MAX;

		if (cooldown_actual > 0) 
		{
			cooldown_actual--;
		} else 
			{
				if (energia_actual > (energia_promedio * multiplicador_umbral) && energia_actual > energia_minima) 
				{
					float tiempo_segundos = (float)(f * TAMANO_FRAME) / frecuencia_muestreo;
					printf("%.3f;0.00;-1\n", tiempo_segundos);
					cooldown_actual = COOLDOWN_FRAMES;
				}
			}

		historial_energia[indice_historial] = energia_actual;
		indice_historial = (indice_historial + 1) % HISTORIAL_MAX;
	}

	free(audio_mono);
	free(frame_entrada);
	free(frame_salida);
	kiss_fftr_free(cfg);

	return 0;
}

float calcular_magnitud(kiss_fft_cpx c)
{
    return sqrtf((c.r * c.r) + (c.i * c.i));
}

