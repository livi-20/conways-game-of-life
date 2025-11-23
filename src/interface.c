#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "game.h"
#include "interface.h"

//Crear función para la cuadrícula
void dibujarCuadricula(WINDOW *win, Cuadricula *cuad) {
    int maxY, maxX;
    getmaxyx(win, maxY, maxX);

    //dejamos el espacio para el titulo (fila arriba) y los botones (fila abajo)
    int offsetY = 2;
    int alto_panel = 3;
    int limiteY = maxY - alto_panel-1;

    //recorrer cada fila de la cuadrícula sin salirse de la matriz ni ponerse encima del titulo ni los botones
    for (unsigned short y = 0; y < cuad->alto && (y + offsetY) < limiteY; y++) {
        for (unsigned short x = 0; x < cuad->ancho && x < (unsigned short)maxX - 2; x++) {
            if (cuad->genActual[y][x])
                mvwprintw(win, y + offsetY, x + 1, "*"); //célula viva
            else
                mvwprintw(win, y + offsetY, x + 1, " "); //célula muerta
        }
    }
}

//Se hace mejor una función para la caja del panel inferior
void actualizarPanel(WINDOW *ventana, int inicio_panel, int win_ancho, int generacion, int delay_ms, int correr){
    int fila_estado = inicio_panel +1;
    int fila_botones = inicio_panel + 2;

    //Para redibujar la línea superior
    mvwhline(ventana, inicio_panel, 1, ACS_HLINE, win_ancho -2);

    for (int x = 1; x < win_ancho -1; x++){
        mvwaddch(ventana,fila_estado, x, ' ');
        mvwaddch(ventana,fila_botones, x, ' ');
    }

    //Linea para los estados
    mvwprintw(ventana, fila_estado, 2, "Gen:%d | Velocidad:%d ms | Estado: %s", generacion, delay_ms, correr ? "RUNNUNG":"PAUSADO");

    //Linea para los botones
    mvwprintw(ventana, fila_botones, 2, "[Q]:Salir | [P]:Play/Pausa | [+]:Acelerar | [-]:Desacelerar | [SPACE]:Avanzar gen");
}

int main (int argc, char ** argv) 
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    //Añadimos la cuadrícula
    Cuadricula *cuadricula = crearCuadricula(ANCHO_CUADRICULA, ALTO_CUADRICULA);
    if (cuadricula == NULL) {
        endwin();
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

    int alto_panel = 3;
    int inicio_panel = win_altura - alto_panel -1; //Por el borde inferior
    
    mvwhline(ventana, 1, 1, ACS_HLINE, win_ancho -2); //poner lineas separadoras
   

    //Para botones dentro de la caja
    int botones = inicio_panel +1;
    mvwprintw(ventana, botones, 2, "[Q]:Salir");
    mvwprintw(ventana, botones, 12, "[P]:Play/Pausa");
    mvwprintw(ventana, botones, 27, "[+]:Acelerar");
    mvwprintw(ventana, botones, 40, "[-]:Desacelerar");
    mvwprintw(ventana, botones, 57, "[SPACE]:Avanzar generación");

    dibujarCuadricula(ventana, cuadricula);
    wrefresh(ventana);

    //Variables para el estado del juego
    int correr = 0; // 0 = pausa, 1 = play
    int delay_ms = 200; //Tiempo entre generaciones en ms (Velocidad inicial = 200 ms)
    int generacion = 0; //Contador de generaciones
    nodelay(ventana, TRUE); //Para que el wgetch no se bloquee esperando una tecla
    
    //Dibujar cajita de estados y botones
    actualizarPanel(ventana, inicio_panel, win_ancho, generacion, delay_ms, correr);

    //Funcionamiento de las teclas completo
    int ch;
    while (1) {
        ch = wgetch(ventana);
        //Entrar solo si hay una tecla
        if (ch != ERR) {
            if (ch == 'q') { //q Salir
                break;
            } else if (ch == ' ') { //SPACE avanzar una generación
                calcularCuadriculaSiguiente(cuadricula);
                generacion++;
                dibujarCuadricula(ventana, cuadricula);
                actualizarPanel(ventana, inicio_panel, win_ancho, generacion, delay_ms, correr);
                wrefresh(ventana);
            } else if (ch == 'p') { //p Play o Pausa 
                correr = !correr; //Alternanr entre 0 y 1
                actualizarPanel(ventana, inicio_panel, win_ancho, generacion, delay_ms, correr);
            } else if (ch == '+') { //+ Acelerar (se reduce el delay)
                if (delay_ms > 50 ){
                    delay_ms -= 50;
                    actualizarPanel(ventana, inicio_panel, win_ancho, generacion, delay_ms, correr);
                }
            } else if (ch == '-') { //- Desaelerar (aumenta el delay)
                if (delay_ms < 1000 ){
                    delay_ms += 50;
                    actualizarPanel(ventana, inicio_panel, win_ancho, generacion, delay_ms, correr);
                }
            }
        }
        //Cuando esta en modo correr, que avance automáticamente
        if (correr) {
            calcularCuadriculaSiguiente(cuadricula);
            generacion++;
            dibujarCuadricula(ventana, cuadricula);
            actualizarPanel(ventana, inicio_panel, win_ancho, generacion, delay_ms, correr);
            wrefresh(ventana);
        }
        napms(delay_ms);
    }

    //Para liberar la memoria de la cuadricula
    liberarCuadricula(cuadricula);
    endwin();
    return 0;
}