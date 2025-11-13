#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/game.h"

//  ================================================
//  Conway's Game of Life - Lógica del Juego
//  ================================================
//  Este módulo implementa la lógica central del Juego de la Vida de Conway:
//      - Gestión de memoria para manipular la cuadrícula de células vivas y muertas.
//      - Aplicación de las reglas de evolución de las células.
//      - Cálculo de las generaciones sucesivas.

//  CARACTERÍSTICAS PRINCIPALES:
//  1. Doble Buffering:
//      - genActual: Matriz que representa la generación actual de células (el estado actual de la cuadrícula).
//      - genSiguiente: Buffer de escritura para la siguiente generación
//      - Esto permite evitar conflictos al leer y escribir el estado de las células en cada generación.

//  2. Wrapping Toroidal:
//  La cuadrícula se "envuelve" en los bordes para realizar el cálculo de los vecinos vivos:
//      - Una célula en el borde derecho tiene vecinos en el borde izquierdo, y viceversa.
//      - Una célula en el borde superior tiene vecinos en el borde inferior, y viceversa.

// 3. Inicialización Aleatoria:
//      - La cuadrícula se inicializa con un 20% de células vivas, distribuidas aleatoriamente.
//      - Se utiliza la función rand() para generar posiciones aleatorias.


// Función para crear una nueva cuadrícula con las dimensiones especificadas y un 20% de células vivas iniciales (aleatorias).
Cuadricula* crearCuadricula(unsigned short ancho, unsigned short alto) {
    // Asignamos memoria para la estructura Cuadricula
    Cuadricula* cuadricula = (Cuadricula*)malloc(sizeof(Cuadricula));

    // Inicializamos los primeros atributos de la estructura.
    cuadricula->ancho = ancho;
    cuadricula->alto = alto;
    cuadricula->numGeneracion = 0;

    // Asignamos memoria para la matriz de células actual. Para ello, se multiplica el tamaño de un puntero a un booleano (que corresponde a una fila de la matriz) por el número de filas (alto).
    cuadricula->genActual = (bool**)malloc(alto * sizeof(bool*));
    if (cuadricula->genActual == NULL) {
        free(cuadricula);
        return NULL;
    }

    // Asignamos memoria para cada fila de la matriz de células actual. Para cada fila, se multiplica el tamaño de un booleano (que corresponde a una célula) por el número de columnas (ancho).
    for (unsigned short i = 0; i < alto; i++) {
        cuadricula->genActual[i] = (bool*)malloc(ancho * sizeof(bool));
        // Liberamos la memoria correspondiente a las filas asignadas y a la cuadrícula en caso de error.
        if (cuadricula->genActual[i] == NULL) {
            for (unsigned short j = 0; j < i; j++) {
                free(cuadricula->genActual[j]);
            }
            free(cuadricula->genActual);
            free(cuadricula);
            return NULL;
        }
    }

    // Asignamos memoria para la matriz de células siguiente (buffer de escritura).
    cuadricula->genSiguiente = (bool**)malloc(alto * sizeof(bool*));
    if (cuadricula->genSiguiente == NULL) {
        for (unsigned short i = 0; i < alto; i++) {
            free(cuadricula->genActual[i]);
        }
        free(cuadricula->genActual);
        free(cuadricula);
        return NULL;
    }

    // Asignamos memoria para cada fila de la matriz de células siguiente. Usamos calloc para inicializar todas las células a false (muertas).
    for (unsigned short i = 0; i < alto; i++) {
        cuadricula->genSiguiente[i] = (bool*)calloc(ancho, sizeof(bool));
        // Liberamos la memoria correspondiente a las filas asignadas y a la cuadrícula en caso de error.
        if (cuadricula->genSiguiente[i] == NULL) {
            for (unsigned short j = 0; j < i; j++) {
                free(cuadricula->genSiguiente[j]);
            }
            free(cuadricula->genSiguiente);
            for (unsigned short j = 0; j < alto; j++) {
                free(cuadricula->genActual[j]);
            }
            free(cuadricula->genActual);
            free(cuadricula);
            return NULL;
        }
    }

    // Usamos srand() y time() para inicializar la semilla del generador de números aleatorios, lo que permite obtener diferentes configuraciones iniciales en cada ejecución del programa.
    srand((unsigned int)time(NULL));

    // Inicializamos la matriz de células actual con un ~20% de células vivas distribuidas aleatoriamente, a través del macro PORCENTAJE_CELULAS_VIVAS_INICIAL.
    for (unsigned short i = 0; i < alto; i++) {
        for (unsigned short j = 0; j < ancho; j++) {
            cuadricula->genActual[i][j] = PORCENTAJE_CELULAS_VIVAS_INICIAL(20);
        }
    }
    return cuadricula;
}

// Función para liberar la memoria asignada a una cuadrícula, incluyendo las matrices de células actuales y siguientes.
void liberarCuadricula(Cuadricula* cuadricula) {
    if (cuadricula->genActual != NULL) {
        for (unsigned short i = 0; i < cuadricula->alto; i++) {
            free(cuadricula->genActual[i]);
        }
        free(cuadricula->genActual);
    }
    if (cuadricula->genSiguiente != NULL) {
        for (unsigned short i = 0; i < cuadricula->alto; i++) {
            free(cuadricula->genSiguiente[i]);
        }
        free(cuadricula->genSiguiente);
    }
    free(cuadricula);
}

// Función para contar el número de células vivas alrededor de una célula específica, considerando el wrapping toroidal.
unsigned short contarVecinasVivas(Cuadricula* cuadricula, unsigned short x, unsigned short y) {
    // Verificamos que la cuadrícula no esté vacía.
    if (cuadricula == NULL) {
        return 0;
    }

    unsigned short vecinasVivas = 0; // Contador de células vivas alrededor de la célula en (x, y).

    // Recorremos las 8 posiciones alrededor de la célula en (x, y), usando las posiciones relativas de cada célula vecina (dx, dy) con respecto a la posición relativa de la célula que estamos evaluando (0, 0).

    //  POSICIONES RELATIVAS DE LAS CÉLULAS VECINAS:
    //  (-1,-1) (0,-1) (+1,-1)
    //  (-1, 0) CÉLULA (+1, 0)
    //  (-1,+1) (0,+1) (+1,+1)

    for (short dy = -1; dy <= 1; dy++) {
        for (short dx = -1; dx <= 1; dx++) {
            // Saltamos la célula que estamos evaluando (la célula en (x, y)).
            if (dx == 0 && dy == 0) {
                continue;
            }

            // Calculamos las coordenadas de la célula vecina, aplicando el wrapping toroidal.
            unsigned short vecinoX = (x + dx + cuadricula->ancho) % cuadricula->ancho;
            unsigned short vecinoY = (y + dy + cuadricula->alto) % cuadricula->alto;

            // Si la célula vecina está viva, incrementamos el contador.
            if (cuadricula->genActual[vecinoY][vecinoX] == true) {
                vecinasVivas++;
            }
        }
    }
    return vecinasVivas;
}
