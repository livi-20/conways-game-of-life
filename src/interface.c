//Version 1
#include <pdcurses.h>
#include <stdio.h>
#include <ncurses.h>

int main (int argc, char ** argv) 
{
    initscr();
    int altura, ancho, pos_y, pos_x;
    altura = 100;
    ancho = 100;
    pos_y = pos_x = 1;

    WINDOW * ventana = newwin(altura, ancho, pos_y, pos_x);
    refresh();

    box(ventana, 0, 0);
    mvwprintw(ventana, 50, 50, "Prueba");
    wrefresh(ventana);

    int c = getch();

    endwin();


    return 0;
}