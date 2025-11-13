//Version 2; mejor código
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

int main (int argc, char ** argv) 
{
    initscr();
    cbreak();

    int altura, ancho;
    
    int margen = 1;
    WINDOW * ventana = newwin(altura -2*margen, ancho - 2*margen, margen, margen);
    box(ventana, 0, 0);
    wrefresh(ventana);

    mvwprintw(ventana, 0, 2, "Conway's Game of Life");
    mvwprintw(ventana, altura - 4, 2, "Presione 'q' para salir");

    int ch;
    while ((ch = wgetch(ventana)) != 'q'){
    }

    endwin();
    return 0;
}