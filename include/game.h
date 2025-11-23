#pragma once
#include <stdbool.h>
#include <stdint.h>

// Este archivo contiene las definiciones y prototipos necesarios para implementar la lógica del Juego de la Vida de Conway, incluyendo la representación de la cuadrícula, las reglas del juego y la evolución de las generaciones.

// Usamos macros para definir las dimensiones de la cuadrícula.
#define ANCHO_CUADRICULA 200
#define ALTO_CUADRICULA 50
#define PORCENTAJE_CELULAS_VIVAS_INICIAL(porcentaje) ((rand() % 100) < porcentaje)

// Definición de la estructura para representar el estado del juego (que corresponde a una cuadrícula de células vivas y muertas).
typedef struct {
    unsigned short ancho;
    unsigned short alto;
    uint64_t numGeneracion; // Número de generación actual
    bool **genActual;  // Generación actual
    bool **genSiguiente;  // Generación siguiente
    // NOTA: Definimos punteros dobles para representar matrices de booleanos con el estado actual y siguiente de cada célula. Usamos punteros para permitir la asignación dinámica de memoria.
} Cuadricula;

// PROTOTIPOS DE FUNCIONES PARA MANEJAR LA CUADRÍCULA Y LA LÓGICA DEL JUEGO

// Función para crear una nueva cuadrícula con las dimensiones especificadas y un 20% de células vivas iniciales (aleatorias).
Cuadricula* crearCuadricula(unsigned short ancho, unsigned short alto);

// Función para liberar la memoria asignada a una cuadrícula.
void liberarCuadricula(Cuadricula* cuadricula);

// Función para calcular la siguiente generación de la cuadrícula según las reglas del Juego de la Vida.
void calcularCuadriculaSiguiente(Cuadricula* cuadricula);

// Función para obtener el estado de una célula específica en la cuadrícula.
bool obtenerEstadoCelula(Cuadricula* cuadricula, unsigned short x, unsigned short y);

// Función para contar el número de células vivas alrededor de una célula específica.
unsigned short contarVecinasVivas(Cuadricula* cuadricula, unsigned short x, unsigned short y);

// Función para obtener el número de generación actual.
uint64_t obtenerNumGeneracion(Cuadricula* cuadricula);

// Función para restablecer la cuadrícula a un estado inicial (con un ~20% de células vivas). Esto puede ser útil para reiniciar el juego sin tener que destruir y recrear la cuadrícula, lo que sería menos eficiente en términos de rendimiento y uso de memoria.
void reiniciarCuadricula(Cuadricula* cuadricula);
