#include <ncurses.h>
#include "../include/game.h"
#include "../include/interface.h"

//  ================================================
//  Conway's Game of Life - Programa Principal
//  ================================================
//  Este programa organiza la ejecución del Juego de la Vida de Conway, cuya lógica e interfaz se encuentran en los módulos game.c e interface.c.
//  El programa principal gestiona la inicialización, el loop principal de ejecución y la limpieza de recursos al finalizar el juego.

// Macros para definir las dimensiones mínimas de la terminal necesarias para mostrar la cuadrícula.
#define ALTURA_MINIMA_TERMINAL 20    // Altura mínima requerida de la terminal para mostrar la cuadrícula.
#define ANCHO_MINIMO_TERMINAL 90     // Ancho mínimo requerido de la terminal para mostrar la cuadrícula.

// Función principal del programa.
int main(void) {
    // Inicializamos la interfaz de usuario a través de ncurses.
    inicializarInterfaz();
    // Creamos la ventana principal para mostrar el juego.
    WINDOW* ventana = crearVentana();
    if (ventana == NULL) {
        cerrarInterfaz();
        fprintf(stderr, "Error al crear la ventana de ncurses.\n");
        return 1;
    }

    // Verificamos que la terminal tenga el tamaño mínimo requerido para mostrar la cuadrícula.
    int alturaTerminal, anchoTerminal;
    getmaxyx(stdscr, alturaTerminal, anchoTerminal);

    if (alturaTerminal < ALTURA_MINIMA_TERMINAL || anchoTerminal < ANCHO_MINIMO_TERMINAL) {
        cerrarVentana(ventana);
        cerrarInterfaz();
        fprintf(
            stderr,
            "La terminal es demasiado pequeña. Se requieren al menos %d filas y %d columnas.\n",
            ALTURA_MINIMA_TERMINAL,
            ANCHO_MINIMO_TERMINAL
        );
        return 1;
    }

    // Creamos la cuadrícula del juego con dimensiones definidas.
    Cuadricula* cuadricula = crearCuadricula(ANCHO_CUADRICULA, ALTO_CUADRICULA);
    if (cuadricula == NULL) {
        cerrarVentana(ventana);
        cerrarInterfaz();
        fprintf(stderr, "No se pudo crear la cuadrícula.\n");
        return 1;
    }
    // Configuramos las variables iniciales del juego.
    bool enEjecucion = false; // Indica si el juego está en ejecución (true) o en pausa (false).
    int velocidad = VELOCIDAD_DEFECTO; // Velocidad de evolución del juego (en ms).

    // Dibujamos la cuadrícula inicial en la ventana.
    dibujarCuadricula(ventana, cuadricula);
    // Mostramos el panel de estado y controles.
    mostrarPanelEstado(ventana, obtenerNumGeneracion(cuadricula), velocidad, enEjecucion);
    // Actualizamos la ventana para reflejar los cambios.
    actualizarVentana(ventana);

    // Bucle principal del juego.
    int tecla; // Definimos una variable para almacenar la tecla presionada.
    bool salir = false; // Variable para controlar la salida del bucle.

    while (salir == false) {
        // Verificamos si hay una tecla presionada.
        tecla = wgetch(ventana);

        if (tecla != ERR) {
            switch (tecla) {
                case 'q':
                case 'Q':
                    salir = true; // Salimos del bucle si se presiona 'q' o 'Q'.
                    break;
                case 'p':
                case 'P':
                    enEjecucion = !enEjecucion; // Alternamos entre ejecución y pausa.
                    break;
                case ' ':
                    if (!enEjecucion) {
                        calcularCuadriculaSiguiente(cuadricula); // Avanzamos una generación si está en pausa.
                    }
                    break;
                case '+':
                case '=':
                    if (velocidad > VELOCIDAD_MAXIMA) {
                        velocidad -= VELOCIDAD_PASO; // Aumentamos la velocidad (disminuimos el delay).
                    }
                    break;
                case '-':
                case '_':
                    if (velocidad < VELOCIDAD_MINIMA) {
                        velocidad += VELOCIDAD_PASO; // Disminuimos la velocidad (aumentamos el delay).
                    }
                    break;
                case 'r':
                case 'R':
                    reiniciarCuadricula(cuadricula); // Reiniciamos la cuadrícula.
                    enEjecucion = false; // Ponemos el juego en pausa.
                    break;
                default:
                    break;
            }
        }
        // Si el juego está en ejecución, calculamos la siguiente generación.
        if (enEjecucion) {
            calcularCuadriculaSiguiente(cuadricula);
        }
        // Actualizamos la cuadrícula y el panel de estado.
        dibujarCuadricula(ventana, cuadricula);
        mostrarPanelEstado(ventana, obtenerNumGeneracion(cuadricula), velocidad, enEjecucion);
        actualizarVentana(ventana);

        // Designamos un tiempo de espera según el estado de ejecución y la velocidad.
        if (enEjecucion) {
            napms(velocidad); // Esperamos el tiempo definido por la velocidad.
        } else {
            napms(50); // Espera corta en pausa para evitar uso excesivo de CPU.
        }
    }
    // Liberamos los recursos antes de salir.
    liberarCuadricula(cuadricula);
    cerrarVentana(ventana);
    cerrarInterfaz();

    return 0;
}
