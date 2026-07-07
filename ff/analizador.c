#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "stb_vorbis.c"
#include "kiss_fftr.h"

#define TAMANO_FRAME 1024
#define VENTANA_LOCAL 10  /* 10 frames hacia atras y hacia adelante para el promedio movil */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float calcular_magnitud(kiss_fft_cpx c);
float fmaxf_custom(float a, float b) { return (a > b) ? a : b; }

int main(int argc, char **argv)
{
	if (argc < 3) {
		fprintf(stderr, "Uso: %s <archivo.ogg> <dificultad_1_a_3>\n", argv[0]);
		return 1;
	}

	int dificultad = atoi(argv[2]);
	float alpha_multiplicador;
	float gamma_ruido_base = 2.0f; /* Filtro anti-ruido estático */

	switch(dificultad) {
		case 1: alpha_multiplicador = 2.5f; break; /* Fácil: Solo impactos muy por encima de la media */
		case 2: alpha_multiplicador = 1.6f; break; /* Normal */
		case 3: alpha_multiplicador = 1.1f; break; /* Difícil: Atrapa hi-hats y sincopas menores */
		default: alpha_multiplicador = 1.6f; break;
	}

	int canales, frecuencia_muestreo;
	short *audio_pcm;

	int total_muestras = stb_vorbis_decode_filename(argv[1], &canales, &frecuencia_muestreo, &audio_pcm);
	if (total_muestras < 0) {
		fprintf(stderr, "Error al decodificar OGG.\n");
		return 1;
	}

	float *audio_mono = malloc(total_muestras * sizeof(float));
	for (int i = 0; i < total_muestras; i++) {
		audio_mono[i] = (float)audio_pcm[i * canales] / 32768.0f; 
	}
	free(audio_pcm);

	kiss_fftr_cfg cfg = kiss_fftr_alloc(TAMANO_FRAME, 0, NULL, NULL);
	kiss_fft_scalar *frame_entrada = malloc(TAMANO_FRAME * sizeof(kiss_fft_scalar));
	kiss_fft_cpx *frame_salida = malloc((TAMANO_FRAME / 2 + 1) * sizeof(kiss_fft_cpx));

	int total_frames = total_muestras / TAMANO_FRAME;
	int bins = TAMANO_FRAME / 2;
	
	float *espectro = calloc(total_frames * bins, sizeof(float));
	float *flujo_espectral = calloc(total_frames, sizeof(float));
	float *umbral_adaptativo = calloc(total_frames, sizeof(float));

	/* PASADA 1: Computar Espectrograma Completo  */
	for (int f = 0; f < total_frames; f++) {
		for (int i = 0; i < TAMANO_FRAME; i++) {
			float muestra = audio_mono[(f * TAMANO_FRAME) + i];
			float multiplicador_ventana = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * i / (TAMANO_FRAME - 1)));
			frame_entrada[i] = muestra * multiplicador_ventana;
		}
		
		kiss_fftr(cfg, frame_entrada, frame_salida);
		
		for (int b = 1; b < bins; b++) {
			espectro[(f * bins) + b] = calcular_magnitud(frame_salida[b]);
		}
	}

	/* PASADA 2: Calcular Flujo Espectral (Cambios positivos de energía)  */
	for (int f = 1; f < total_frames; f++) {
		float flujo = 0.0f;
		for (int b = 1; b < bins; b++) {
			float diff = espectro[(f * bins) + b] - espectro[((f - 1) * bins) + b];
			flujo += fmaxf_custom(0.0f, diff);
		}
		flujo_espectral[f] = flujo;
	}

	/* PASADA 3: Calcular Umbral Adaptativo Local  */
	for (int f = 0; f < total_frames; f++) {
		int inicio = (f - VENTANA_LOCAL < 0) ? 0 : f - VENTANA_LOCAL;
		int fin = (f + VENTANA_LOCAL >= total_frames) ? total_frames - 1 : f + VENTANA_LOCAL;
		
		float suma_local = 0.0f;
		int contador = 0;
		for (int w = inicio; w <= fin; w++) {
			suma_local += flujo_espectral[w];
			contador++;
		}
		
		float media_local = suma_local / contador;
		umbral_adaptativo[f] = (media_local * alpha_multiplicador) + gamma_ruido_base;
	}

	printf("# Analizando %s (%d Hz)\n", argv[1], frecuencia_muestreo);
	printf("# Algoritmo: Flujo Espectral + Peak Picking Adaptativo\n");
	printf("# tiempo;duracion;carril\n");

	/*  PASADA 4: Peak y Generación de Nivel  */
	for (int f = 1; f < total_frames - 1; f++) {
		float sf_actual = flujo_espectral[f];
		
		/* Condición estricta de máximo local y superación del umbral */
		if (sf_actual >= umbral_adaptativo[f] && 
		    sf_actual > flujo_espectral[f - 1] && 
		    sf_actual > flujo_espectral[f + 1]) 
		{
			/* Se elimina la necesidad del cooldown, la propia forma de la onda dicta la separación */
			float tiempo_segundos = (float)(f * TAMANO_FRAME) / frecuencia_muestreo;
			printf("%.3f;0.00;-1\n", tiempo_segundos);
		}
	}

	free(umbral_adaptativo);
	free(flujo_espectral);
	free(espectro);
	free(audio_mono);
	free(frame_entrada);
	free(frame_salida);
	kiss_fftr_free(cfg);

	return 0;
}

float calcular_magnitud(kiss_fft_cpx c) {
	return sqrtf((c.r * c.r) + (c.i * c.i));
}
