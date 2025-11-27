# Conway's Game of Life (Juego de la vida de Conway)

Este proyecto consiste en la implementación del juego de la vida de Conway en el lenguaje de programación `C`, utilizando la biblioteca `ncurses`. 

El juego es un _autómata celular de cero jugadores_; una simulación automática donde se puede visualizar la evolución de "células" que cumplen ciertas reglas para su nacimiento, supervivencia y muerte en cada generación sucesiva, y que se representan dentro de una cuadrícula.

La **interfaz de usuario** usa la biblioteca `ncurses` para su funcionamiento, y permite:
- Controlar la velocidad del tiempo que pasa entre cada generación.
- Reiniciar el juego para probar otro patrón inicial.
- Avanzar una generación (cuando el juego está en pausa).
- Visualizar la información del estado de juego.
- Pausar, resumir o salir de la simulación.


## Estructura del proyecto
Se organizó el proyecto en dos grandes módulos (o _features_) para facilitar el trabajo en paralelo, que corresponden a la **lógica del juego** y a la **interfaz de usuario**.

El repositorio está estructurado de la siguiente manera:

```
conways-game-of-life/
├── include/
│   ├── game.h           # Macros y prototipos de funciones para la lógica del juego.
│   └── interface.h      # Macros y prototipos de funciones para la interfaz de usuario.
├── src/
│   ├── main.c           # Programa principal de demostración.
│   ├── game.c           # Implementación de las funciones de la lógica del juego.
│   └── interface.c      # Implementación de las funciones de la interfaz de usuario.
├── bin/                 # Directorio para el ejecutable (generado automáticamente)
├── obj/                 # Directorio para archivos objeto (generado automáticamente)
├── Makefile             # Automatización de compilación y ejecución
└── README.md            # Este archivo
```


### `Game`
Contiene la lógica del juego, con funciones que permiten:
- Crear y liberar la cuadrícula donde se muestran las células.
- Realizar el conteo de las células vecinas vivas que tiene cada célula individual (con esto aplicamos la lógica del juego).
- Calcular la cuadrícula siguiente y almacenarla en un buffer; donde se muestra si una célula sobrevive o muere.

### `Interface`
Implementa la interfaz de usuario en la terminal, gracias a `ncurses`. También se designan funciones para:
- Inicializar la pantalla en la terminal y la creación de la ventana principal.
- Dibujar la cuadrícula con caracteres específicos para las células vivas o muertas.
- Crear y actualizar el panel inferior, donde se muestra el estado de la simulación y los controles disponibles.

### `Main`
Coordina toda la aplicación para demostrar la funcionalidad del juego. El proceso involucra:
- Inicializar ncurses y la cuadrícula.
- Controlar el bucle principal del juego.
- Procesar las teclas ingresadas por el usuario.
- Actualizar la simulación cuadro por cuadro.


## Compatibilidad, Dependencias y Bibliotecas
El proyecto es compatible con Linux y MacOS. Para compilar y ejecutar correctamente el programa, es necesario contar con las siguientes dependencias:

### 1. Biblioteca `ncurses`
El proyecto utiliza `ncurses` para manejar la interfaz gráfica en la terminal.

**Instalación: Debian-based distros:**
```bash
sudo apt install libncursesw5-dev
```
**Instalación: Red Hat-based distros:**
```bash
sudo dnf install ncurses-devel
```
**Instalación: MacOS (Homebrew):**
```zsh
brew install ncurses
```
---

### 2. Compilador `GCC`
Se utiliza `GCC` para compilar este proyecto.

**Instalación: Debian-based distros:**
```bash
sudo apt install gcc
```
**Instalación: Red Hat-based distros:**
```bash
sudo dnf install gcc
```
**Instalación: MacOS (Homebrew):**
```zsh
brew install gcc
```

---

### 3. `Make`
El proyecto utiliza `Make` como sistema automatizado para la compilación, ejecución y mantenimiento del proyecto a través de un `Makefile`. 

**Instalación: Debian-based distros:**
```bash
sudo apt install make
```
**Instalación: Red Hat-based distros:**
```bash
sudo dnf install make
```
**Instalación: MacOS (Homebrew):**
```zsh
brew install make
```
---

## Compilación y ejecución
Este proyecto cuenta con un sistema automatizado para la compilación, ejecución y mantenimiento del proyecto a través de un `Makefile`, por lo que no es necesario compilar los archivos manualmente.

### Compilación
Para compilar, abra una ventana de terminal y diríjase al directorio del proyecto. Ejecute el comando:
```bash
make
```

Este comando realiza las siguientes acciones: 
1. Crea los directorios `/obj` y `/bin`
2. Compila todos los archivos `.c`
3. Genera el ejecutable `conway` en `bin/conway`

### Ejecución
Para correr el programa una vez compilado, ejecute:
```bash
make run
```

### Limpiar archivos generados
Para limpiar los archivos y directorios de compilación generados por `Make`, ejecute:
```bash
make clean
```

### Ejecutar con Valgrind
Este comando corre el ejecutable usando Valgrind, y genera el archivo `valgrind-report.txt` para revisar el uso de memoria y las posibles fugas.
```bash
make valgrind
```
