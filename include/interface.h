#ifndef INTERFACE_H
#define INTERFACE_H
#include <ncurses.h>
#include "game.h"

void dibujarCuadricula(WINDOW *win, Cuadricula *cuad);

void actualizarPanel(WINDOW *ventana, int inicio_panel, int win_ancho, int generacion, int delay_ms, int correr);

#endif