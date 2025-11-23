# ------------------------------------------------------------------
# Makefile para compilar el proyecto del Juego de la Vida de Conway
# ------------------------------------------------------------------

# Compilador y flags de compilación
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -g

# Detectar el sistema operativo
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS
    LDFLAGS = -lncurses
else
    # Linux y otros
    LDFLAGS = -lncursesw
endif

# Directorios
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Archivos
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/game.c $(SRC_DIR)/interface.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
HEADERS = $(INC_DIR)/game.h $(INC_DIR)/interface.h
TARGET = $(BIN_DIR)/conway

# Regla de compilación por defecto
all: $(TARGET)
	@echo ""
	@echo "--- COMPILACIÓN COMPLETADA ---"
	@echo ""
	@echo "Para ejecutar el programa, usa 'make run'."
	@echo "Para ejecutar con valgrind, usa 'make valgrind'."
	@echo "Para limpiar los archivos generados, usa 'make clean'."
	@echo ""

# Regla para crear el ejecutable final
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	@echo "> Enlazando archivos objeto..."
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilar archivos .c a .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	@echo "> Compilando $<..."
	@$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Crear directorio bin si no existe
$(BIN_DIR):
	@echo "> Creando directorio bin..."
	@mkdir -p $(BIN_DIR)

# Crear directorio obj si no existe
$(OBJ_DIR):
	@echo "> Creando directorio obj..."
	@mkdir -p $(OBJ_DIR)

# Regla para compilar y ejecutar el programa
run: $(TARGET)
	@echo ""
	@echo "--- EJECUTANDO PROGRAMA ---"
	@echo ""
	@./$(TARGET)

# Regla para ejecutar el programa con Valgrind (detección de memory leaks)
valgrind: $(TARGET)
	@echo ""
	@echo "--- EJECUTANDO PROGRAMA CON VALGRIND ---"
	@echo ""
	@echo "NOTA: El reporte se guardará en valgrind-report.txt"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-report.txt ./$(TARGET)
	@echo ""
	@echo " RESUMEN RÁPIDO DE VALGRIND "
	@grep "ERROR SUMMARY" valgrind-report.txt
	@grep "in use at exit" valgrind-report.txt
	@echo ""

# Regla para limpiar los archivos generados
clean:
	@echo "> Limpiando archivos generados..."
	@echo ""
	@rm -rf $(BIN_DIR) $(OBJ_DIR)
	@echo "--- LIMPIEZA COMPLETADA ---"
	@echo ""

# Regla para mostrar ayuda
help:
	@echo ""
	@echo "----------------------------------------------------------"
	@echo "Makefile para el Proyecto del Juego de la Vida de Conway"
	@echo "----------------------------------------------------------"
	@echo ""
	@echo "Comandos disponibles:"
	@echo "  make              - Compilar el proyecto"
	@echo "  make run          - Compilar y ejecutar el programa"
	@echo "  make clean        - Limpiar archivos compilados"
	@echo "  make valgrind     - Ejecutar con Valgrind (memory leak check)"
	@echo "  make help         - Mostrar esta ayuda"
	@echo ""

# Marcar las reglas que no corresponden a archivos
.PHONY: all run valgrind clean help
