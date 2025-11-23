#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <ncurses.h>
#include "game.h"

// Este archivo contiene las definiciones y prototipos necesarios para implementar la interfaz de usuario del Juego de la Vida de Conway, utilizando la biblioteca ncurses para la representación visual en la terminal.

// Usamos macros para definir la velocidad de evolución mínima y máxima del juego (en ms)
#define VELOCIDAD_MAXIMA 50     // Velocidad más rápida (~20 generaciones por segundo)
#define VELOCIDAD_MINIMA 1000   // Velocidad más lenta (1 s entre generaciones)
#define VELOCIDAD_DEFECTO 200   // Velocidad por defecto (5 generaciones por segundo)
#define VELOCIDAD_PASO 50       // Paso de ajuste de velocidad (-/+ 50 ms al aumentar o disminuir)

// Función para inicializar la interfaz de usuario (a través de una ventana de ncurses).
void inicializarInterfaz(void);

// Función para limpiar y cerrar la interfaz de usuario.
void cerrarInterfaz(void);

// Función para crear una ventana de ncurses con bordes y título automático.
WINDOW* crearVentana(void);

// Función para eliminar una ventana de ncurses y liberar sus recursos.
void cerrarVentana(WINDOW* ventana);

// Función para dibujar la cuadrícula en la ventana de ncurses.
void dibujarCuadricula(WINDOW* ventana, Cuadricula* cuadricula);

// Función para mostrar el panel de estado y controles en la ventana de ncurses.
void mostrarPanelEstado(WINDOW* ventana, uint64_t numGeneracion, int velocidadEvolucion, bool programaEnEjecucion);

// Función para actualizar la ventana de ncurses para reflejar los cambios realizados en el buffer.
void actualizarVentana(WINDOW* ventana);
