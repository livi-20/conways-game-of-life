//Version 3; más botones
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

int main (int argc, char ** argv) 
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    int altura, ancho;
    getmaxyx(stdscr, altura, ancho);
    
    int margen = 1;
    WINDOW * ventana = newwin(altura -2*margen, ancho - 2*margen, margen, margen);
    box(ventana, 0, 0);
    wrefresh(ventana);
    mvwprintw(ventana, 0, 2, "Conway's Game of Life");

    int botones = altura - 4;
    mvwprintw(ventana, botones, 2, "q: Salir");
    mvwprintw(ventana, botones, 12, "p: Play/Pausa");
    mvwprintw(ventana, botones, 27, "+: Acelerar");
    mvwprintw(ventana, botones, 40, "-: Desacelerar");
    mvwprintw(ventana, botones, 56, "SPACE: Avanzar gen");

    int ch;
    while ((ch = wgetch(ventana)) != 'q'){
    }

    endwin();
    return 0;
}