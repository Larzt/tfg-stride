
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -D TEST

INCLUDES = -I language/lexer -I utils -I consts -I language

LEXER_SRCS = language/lexer/lexer.cc \
             language/lexer/line_lexer.cc

PARSER_SRCS = main/language/parser.cc \
              $(LEXER_SRCS)


all: lexer parser

lexer:
	@echo "Compilando Lexer Test..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/lexer_test.cc $(LEXER_SRCS) -o lexer_test
	@echo "¡Compilado con éxito! Ejecuta con: ./lexer_test"

parser:
	@echo "Compilando Parser Test..."

	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/parser_test.cc $(PARSER_SRCS) -o parser_test
	@echo "¡Compilado con éxito! Ejecuta con: ./parser_test"

clean:
	@echo "Limpiando ejecutables..."
	rm -f lexer_test parser_test
	@echo "Directorio limpio."

.PHONY: all lexer parser clean
