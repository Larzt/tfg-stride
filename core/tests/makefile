# Compilador nativo de tu ordenador
CXX = g++

# Banderas de compilación: C++23 e incluimos las carpetas de headers
CXXFLAGS = -Wall -Wextra -std=c++2b -I../plugin/lexer/include -Iinclude -g

# LDFLAGS = -lgtest -lgtest_main -pthread

# Nombre del ejecutable
TARGET = run_tests

# 1. Buscamos el código de producción
SRCS_LANG = $(wildcard ../plugin/lexer/*.cc)

# 2. Buscamos los archivos de test
SRCS_TESTS = $(wildcard *.cc)

# Unimos todos los archivos a compilar
SRCS = $(SRCS_LANG) $(SRCS_TESTS)

# Transformamos la lista de .cc en .o (archivos objeto)
OBJS = $(SRCS:.cc=.o)

# Regla principal
all: $(TARGET)

# Cómo construir el ejecutable final
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "==================================================="
	@echo "¡Compilación exitosa! Ejecuta los tests con: ./$(TARGET)"
	@echo "==================================================="

# Cómo construir cada .o a partir de su .cc
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar los archivos compilados
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Archivos limpios."
