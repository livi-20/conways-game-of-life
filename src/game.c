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


// Función para crear una nueva cuadrícula con las dimensiones especificadas y ~20% de células vivas iniciales (aleatorizadas).
Cuadricula* crearCuadricula(unsigned short ancho, unsigned short alto) {
    // Asignamos memoria para la estructura Cuadricula
    Cuadricula* cuadricula = (Cuadricula*)malloc(sizeof(Cuadricula));
    if (cuadricula == NULL) {
        return NULL; // Retornamos NULL en caso de error al asignar memoria.
    }
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

    // Inicializamos la matriz de células actual con ~20% de células vivas distribuidas aleatoriamente, a través del macro PORCENTAJE_CELULAS_VIVAS_INICIAL.
    for (unsigned short i = 0; i < alto; i++) {
        for (unsigned short j = 0; j < ancho; j++) {
            cuadricula->genActual[i][j] = PORCENTAJE_CELULAS_VIVAS_INICIAL(20);
        }
    }
    return cuadricula;
}

// Función para liberar la memoria asignada a una cuadrícula, incluyendo las matrices de células actuales y siguientes.
void liberarCuadricula(Cuadricula* cuadricula) {
    // Verificamos que la cuadrícula no esté vacía.
    if (cuadricula == NULL) {
        return;
    }
    // Liberamos la memoria asignada a las matrices genActual y genSiguiente.
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

// Función para calcular la siguiente generación de la cuadrícula según las reglas del Juego de la Vida.
void calcularCuadriculaSiguiente(Cuadricula* cuadricula) {
    // Verificamos que la cuadrícula no esté vacía.
    if (cuadricula == NULL) {
        return;
    }
    // Recorremos cada célula de la cuadrícula actual para aplicar las reglas del Juego de la Vida.
    for (unsigned short y = 0; y < cuadricula->alto; y++) {
        for (unsigned short x = 0; x < cuadricula->ancho; x++) {
            // Contamos el número de células vivas alrededor de la célula en (x, y).
            unsigned short vecinasVivas = contarVecinasVivas(cuadricula, x, y);
            // Aplicamos las reglas del Juego de la Vida para determinar el estado de la célula en la siguiente generación.
            if (cuadricula->genActual[y][x] == true) {
                // Una célula viva con 2 o 3 vecinas vivas sobrevive; de lo contrario, muere.
                if (vecinasVivas == 2 || vecinasVivas == 3) {
                    cuadricula->genSiguiente[y][x] = true; // La célula sobrevive.
                } else {
                    cuadricula->genSiguiente[y][x] = false; // La célula muere.
                }
            } else {
                // Una célula muerta con exactamente 3 vecinas vivas se convierte en una célula viva.
                if (vecinasVivas == 3) {
                    cuadricula->genSiguiente[y][x] = true; // La célula nace.
                } else {
                    cuadricula->genSiguiente[y][x] = false; // La célula permanece muerta.
                }
            }
        }
    }
    // Intercambiamos los punteros de las matrices genActual y genSiguiente para avanzar a la siguiente generación.
    bool** temp = cuadricula->genActual;
    cuadricula->genActual = cuadricula->genSiguiente;
    cuadricula->genSiguiente = temp;

    // Incrementamos el número de generación.
    cuadricula->numGeneracion++;
}

// Función para obtener el estado de una célula específica en la cuadrícula.
bool obtenerEstadoCelula(Cuadricula* cuadricula, unsigned short x, unsigned short y) {
    // Verificamos que la cuadrícula no esté vacía y que las coordenadas estén dentro de los límites.
    if (cuadricula == NULL || x >= cuadricula->ancho || y >= cuadricula->alto) {
        return false; // Retornamos false si la cuadrícula es nula o las coordenadas son inválidas.
    }
    return cuadricula->genActual[y][x]; // Retornamos el estado de la célula en (x, y).
}

// Función para obtener el número de generación actual.
uint64_t obtenerNumGeneracion(Cuadricula* cuadricula) {
    // Verificamos que la cuadrícula no esté vacía.
    if (cuadricula == NULL) {
        return 0;
    }
    return cuadricula->numGeneracion; // Retornamos el número de generación actual.
}

// Función para restablecer la cuadrícula a un estado inicial (con un ~20% de células vivas, aleatorizadas).
void reiniciarCuadricula(Cuadricula* cuadricula) {
    // Verificamos que la cuadrícula no esté vacía.
    if (cuadricula == NULL) {
        return;
    }
    // Limpiamos la matriz de células siguiente (buffer de escritura), reestableciendo todas las células a 'false' (muertas). La función memset se utiliza para establecer todos los bytes de la memoria asignada a 0.
    for (unsigned short i = 0; i < cuadricula->alto; i++) {
        memset(cuadricula->genSiguiente[i], 0, cuadricula->ancho * sizeof(bool));
    }
    // Generamos una nueva semilla para el generador de números aleatorios.
    srand((unsigned int)time(NULL));
    // Inicializamos la matriz de células actual con un ~20% de células vivas distribuidas aleatoriamente.
    for (unsigned short i = 0; i < cuadricula->alto; i++) {
        for (unsigned short j = 0; j < cuadricula->ancho; j++) {
            cuadricula->genActual[i][j] = PORCENTAJE_CELULAS_VIVAS_INICIAL(20);
        }
    }
    // Restablecemos el número de generación a 0.
    cuadricula->numGeneracion = 0;
}
