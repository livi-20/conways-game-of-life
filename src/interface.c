//Version 3; más botones
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "../include/game.h" //Usar cuadrícula puesta en el game.h

//Crear función para la cuadrícula
void dibujarCuadricula(WINDOW *win, Cuadricula *cuad) {
    int maxY, maxX;
    getmaxyx(win, maxY, maxX);

    //dejamos el espacio para el titulo (fila arriba) y los botones (fila abajo)
    int offsetY = 1;
    int limiteY = maxY -2;

    //recorrer cada fila de la cuadrícula sin salirse de la matriz ni ponerse encima del titulo ni los botones
    for (unsigned short y = 0; y < cuad->alto && (y + offsetY) < limiteY; y++) {
        for (unsigned short x = 0; x < cuad->ancho && x < (unsigned short)maxX - 2; x++) {
            if (cuad->genActual[y][x])
                mvwprintw(win, y + offsetY, x + 1, "0"); //célula viva
            else
                mvwprintw(win, y + offsetY, x + 1, " "); //célula muerta
        }
    }
}


int main (int argc, char ** argv) 
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    //Añadimos la cuadrícula
    Cuadricula *cuadricula = crearCuadricual(ANCHO_CUADRICULA, ALTO_CUADRICULA);
    if (cuadricula == NULL) {
        edwin();
        fprintf(stderr, "No se pudo crear la cuadrícula.\n");
        return 1;
    }

    int altura, ancho;
    getmaxyx(stdscr, altura, ancho);
    
    int margen = 1;
    WINDOW * ventana = newwin(altura -2*margen, ancho - 2*margen, margen, margen);
    box(ventana, 0, 0);
    mvwprintw(ventana, 0, 2, "Conway's Game of Life");
    
    //Se hace un cambio para usar el tamaño de la ventana y no la terminal como lo tenía anteriormente.
    int win_altura, win_ancho;
    getmaxyx(ventana, win_altura, win_ancho);
    
    //4 -> 2 por que es la penúltima fila dentro de la ventana
    int botones = altura - 2;
    mvwprintw(ventana, botones, 2, "q: Salir");
    mvwprintw(ventana, botones, 12, "p: Play/Pausa");
    mvwprintw(ventana, botones, 27, "+: Acelerar");
    mvwprintw(ventana, botones, 40, "-: Desacelerar");
    mvwprintw(ventana, botones, 56, "SPACE: Avanzar gen");

    dibujarCuadricula(ventana, cuadricula);
    wrefresh(ventana);
    
    //Funcionamiento de las teclas
    int ch;
    while ((ch = wgetch(ventana)) != 'q'){
        if (ch == ' ') {
            calcularCuadriculaSiguiente(cuadricula);
            dibujarCuadricula(ventana, cuadricula);
            wrefresh(ventana);
        }
    }

    //Para liberar la memoria de la cuadricula
    liberarCuadricula(cuadricula);
    endwin();
    return 0;
}