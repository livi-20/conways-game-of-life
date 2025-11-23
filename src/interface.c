#include <ncurses.h>
#include <locale.h>
#include "../include/interface.h"

//  ================================================
//  Conway's Game of Life - Interfaz de Usuario
//  ================================================
//  Este módulo implementa la interfaz de usuario para el Juego de la Vida de Conway,
//  utilizando la biblioteca ncurses.
//
//  CARACTERÍSTICAS PRINCIPALES:
//  1. Inicialización personalizada de ncurses
//      - noecho(): Desactiva la visualización automática de caracteres ingresados.
//      - cbreak(): Permite la entrada de caracteres sin esperar a un salto de línea.
//      - curs_set(0): Oculta el cursor en la pantalla.
//      - nodelay(): Configura la entrada para que no bloquee la ejecución del programa.
//
// 2. Doble Buffering
//      - Las funciones de dibujo de ncurses no se muestran inmediatamente en pantalla, sino que se almacenan en un buffer.
//      - Esto permite evitar 'parpadeos' y mejorar el rendimiento.
//      - La función actualizarPantalla() refresca la ventana para reflejar los cambios realizados en el buffer.

// Macros para definir la representación visual de las células vivas y muertas en la interfaz de usuario.
#define CELULA_VIVA "█"
#define CELULA_MUERTA " "

// Macros para definir los elementos principales de la ventana y su disposición.
#define MARGEN_VENTANA 1
#define LINEA_TITULO 0              // Fila donde se dibuja el título de la ventana.
#define ESPACIADO_TITULO 2          // Espaciado del título, desde el borde izquierdo.
#define FILA_SEPARADOR 1            // Fila donde se dibuja la línea separadora debajo del título.
#define POSICION_BORDE_IZQUIERDO 0  // Columna del borde izquierdo de la ventana.
#define ANCHO_BORDE 1               // Ancho del borde de la ventana.
#define ALTURA_PANEL_INFERIOR 3     // Altura total del panel inferior (separador, estado y controles).

// Macros para definir la disposición de la cuadrícula dentro de la ventana.
#define INICIO_CUADRICULA_Y 2   // Fila de inicio de la cuadrícula.
#define INICIO_CUADRICULA_X 1   // Columna de inicio de la cuadrícula, con margen izquierdo.

// Función para inicializar la interfaz de usuario (a través de una ventana de ncurses).
void inicializarInterfaz(void) {
    setlocale(LC_ALL, "");  // Configura la localización para permitir caracteres especiales.
    initscr();              // Inicializa la pantalla para ncurses.
    noecho();               // Desactiva la visualización automática de caracteres ingresados.
    cbreak();               // Permite la entrada de caracteres sin esperar a un salto de línea.
    curs_set(0);            // Oculta el cursor en la pantalla.
    nodelay(stdscr, TRUE);  // Configura la entrada para que no bloquee la ejecución del programa.
    keypad(stdscr, TRUE);   // Habilita la captura de teclas especiales (flechas, F1, etc.).
}

// Función para limpiar y cerrar la interfaz de usuario.
void cerrarInterfaz(void) {
    endwin(); // Finaliza ncurses y restaura la terminal a su estado original.
}

// Función para crear una ventana de ncurses con bordes y título automático.
WINDOW* crearVentana(void) {
    int alturaTerminal, anchoTerminal;       // Variables para almacenar las dimensiones de la terminal.
    getmaxyx(stdscr, alturaTerminal, anchoTerminal);    // Obtiene las dimensiones de la terminal y las almacena en las variables correspondientes.

    // Creamos una nueva ventana con márgenes definidos.
    // NOTA: Se resta el doble del margen al ancho y altura totales, ya que el margen se aplica en ambos lados.
    WINDOW* ventana = newwin(
        alturaTerminal - (MARGEN_VENTANA * 2),  // Altura de la ventana (sin márgenes).
        anchoTerminal - (MARGEN_VENTANA * 2),   // Ancho de la ventana (sin márgenes).
        MARGEN_VENTANA,                         // Fila de inicio de la ventana.
        MARGEN_VENTANA                          // Columna de inicio de la ventana.
    );
    if (ventana == NULL) {
        return NULL; // Retorna NULL si no se pudo crear la ventana.
    }
    nodelay(ventana, TRUE); // Configura la ventana para que no bloquee la ejecución del programa al esperar una tecla.
    box(ventana, 0, 0);     // Dibujamos un borde alrededor de la ventana.

    // Añadimos el título en la parte superior de la ventana.
    mvwprintw(ventana, LINEA_TITULO, ESPACIADO_TITULO, " Conway's Game of Life ");

    int anchoVentana = getmaxx(ventana); // Variable para almacenar el ancho de la ventana.

    // Dibujamos una línea separadora debajo del título.
    mvwhline(
        ventana,                            // Ventana donde se dibuja la línea.
        FILA_SEPARADOR,                     // Fila donde se dibuja la línea.
        ANCHO_BORDE,                        // Columna de inicio de la línea.
        ACS_HLINE,                          // Carácter utilizado para dibujar la línea (línea horizontal).
        anchoVentana - (ANCHO_BORDE * 2)    // Longitud de la línea (ancho de la ventana menos los bordes)
    );
    // Conectamos los bordes de la línea separadora con los bordes de la ventana.
    mvwaddch(ventana, FILA_SEPARADOR, POSICION_BORDE_IZQUIERDO, ACS_LTEE);      // Conexión con el borde izquierdo.
    mvwaddch(ventana, FILA_SEPARADOR, anchoVentana - ANCHO_BORDE, ACS_RTEE);    // Conexión con el borde derecho.

    return ventana; // Retornamos el puntero a la ventana creada.
}

// Función para eliminar una ventana de ncurses y liberar sus recursos.
void cerrarVentana(WINDOW* ventana) {
    if (ventana != NULL) {
        delwin(ventana); // Elimina la ventana y libera sus recursos.
    }
}

// Función para dibujar la cuadrícula en la ventana de ncurses.
void dibujarCuadricula(WINDOW* ventana, Cuadricula* cuadricula) {
    if (ventana == NULL || cuadricula == NULL) {
        return; // Retorna si la ventana o la cuadrícula son NULL.
    }
    int alturaVentana, anchoVentana;
    getmaxyx(ventana, alturaVentana, anchoVentana); // Obtiene las dimensiones de la ventana.

    // Calculamos el área disponible para mostrar la cuadrícula dentro de la ventana.
    int alturaDisponible = alturaVentana - ALTURA_PANEL_INFERIOR - INICIO_CUADRICULA_Y - ANCHO_BORDE;
    int anchoDisponible = anchoVentana - (ANCHO_BORDE * 2);

    // Calculamos el área visible de la cuadrícula, limitándola al área disponible en la ventana.
    // NOTA: El operador condicional ? : se utiliza para elegir el valor mínimo entre el tamaño de la cuadrícula y el área disponible. Si la cuadrícula es más pequeña que el área disponible, se muestra completamente; de lo contrario, se limita al área disponible.
    int alturaVisible = (cuadricula->alto < alturaDisponible) ? cuadricula->alto : alturaDisponible;
    int anchoVisible = (cuadricula->ancho < anchoDisponible) ? cuadricula->ancho : anchoDisponible;

    // Recorremos cada célula de la cuadrícula visible y la dibujamos en la ventana.
    for (int y = 0; y < alturaVisible; y++) {
        for (int x = 0; x < anchoVisible; x++) {
            // Obtenemos el estado de la célula en la posición (x, y).
            bool estadoCelula = obtenerEstadoCelula(cuadricula, x, y);
            // Seleccionamos el carácter a dibujar según el estado de la célula.
            char* caracterCelula = estadoCelula ? CELULA_VIVA : CELULA_MUERTA;
            // Dibujamos la célula, ajustando las coordenadas para tener en cuenta el margen y la posición de inicio de la cuadrícula.
            mvwprintw(ventana, INICIO_CUADRICULA_Y + y, INICIO_CUADRICULA_X + x, "%s", caracterCelula);
        }
    }
}

// Función para mostrar y actualizar el panel de estado y controles en la ventana de ncurses.
void mostrarPanelEstado(WINDOW* ventana, uint64_t numGeneracion, int velocidadEvolucion, bool programaEnEjecucion) {
    if (ventana == NULL) {
        return; // Retorna si la ventana es NULL.
    }
    int alturaVentana, anchoVentana;
    getmaxyx(ventana, alturaVentana, anchoVentana); // Obtiene las dimensiones de la ventana.

    // Calculamos la fila de inicio del panel inferior.
    int inicioPanelInferior = alturaVentana - ALTURA_PANEL_INFERIOR - ANCHO_BORDE;

    // Dibujamos la línea separadora del panel inferior.
    mvwhline(ventana, inicioPanelInferior, ANCHO_BORDE, ACS_HLINE, anchoVentana - (ANCHO_BORDE * 2));
    mvwaddch(ventana, inicioPanelInferior, POSICION_BORDE_IZQUIERDO, ACS_LTEE);      // Conexión con el borde izquierdo.
    mvwaddch(ventana, inicioPanelInferior, anchoVentana - ANCHO_BORDE, ACS_RTEE);    // Conexión con el borde derecho.

    int filaEstado = inicioPanelInferior + 1;                        // Fila para mostrar el estado del juego.
    const char* textoEstado = programaEnEjecucion ? "CORRIENDO" : "EN PAUSA";   // Texto para el estado del juego.

    // Limpiamos la fila del estado de juego antes de actualizarla.
    mvwhline(ventana, filaEstado, ANCHO_BORDE, ' ', anchoVentana - (ANCHO_BORDE * 3));

    // Mostramos el estado del juego en la primera línea del panel inferior.
    mvwprintw(ventana, filaEstado, ANCHO_BORDE + 1, "Generación: %llu | Velocidad de Evolución: %d ms | Estado: %s",
        (unsigned long long)numGeneracion, velocidadEvolucion, textoEstado);

    int filaControles = filaEstado + 1; // Fila para mostrar los controles del juego.

    // Mostramos los controles disponibles en la segunda línea del panel inferior.
    mvwprintw(ventana, filaControles, ANCHO_BORDE + 1, "[Q]Salir | [P]Play/Pausa | [-/+]Velocidad | [SPACE]Avanzar generación | [R]Reiniciar");
}

// Función para actualizar la ventana de ncurses para reflejar los cambios realizados en el buffer.
void actualizarVentana(WINDOW* ventana) {
    if (ventana != NULL) {
        wrefresh(ventana); // Refresca la ventana para mostrar los cambios realizados en el buffer.
    }
}
