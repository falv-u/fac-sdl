#include "commons.h"


ESTADO_ACTUAl juego_principal(eventos_globales ev_gl, SDL_Event *evento, ESTADO_ACTUAl estado_actual)
{
        // A. Limpiar la pantalla (establece el color de fondo y borra el buffer)
        SDL_SetRenderDrawColor(ev_gl.renderizado, 0, 0, 0, 255); // Negro
        SDL_RenderClear(ev_gl.renderizado);

        // B. Dibujar geometría (un rectángulo rojo)
        SDL_Rect rectangulo = { 200, 150, 400, 300 }; // {x, y, ancho, alto}
        SDL_SetRenderDrawColor(ev_gl.renderizado, 255, 0, 0, 255); // Rojo
        SDL_RenderFillRect(ev_gl.renderizado, &rectangulo);

        // C. Presentar los cambios en pantalla
        SDL_RenderPresent(ev_gl.renderizado);
        SDL_Delay(16); // ~60 FPS
	return estado_actual;
}



