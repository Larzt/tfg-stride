#include "parser.h"
#include "utility.h"
#include "colors.h"

bool BaseParser::check(TokenType type) const
{
  if (cursor >= tokens.size())
  {
    return false;
  }
  return peek().type == type;
}

void BaseParser::expect(TokenType type, std::string message)
{
  if (check(type))
  {
    advance();
    return;
  }
  Token errorToken = peek();

  std::cerr << RED << "[Error de Sintaxis] " << RESET
            << message << "\n"
            << " Encontrado: '" << errorToken.value << "' (Cursor pos: " << cursor << ")"
            << std::endl;

  exit(1);
}

/**
 * PARSE IF
 */
void ParseIf::parse()
{

  expect(TokenType::IF, "Se esperaba 'if'");

  // std::string conditionVar = peek().value;
  expect(TokenType::IDENTIFIER, "Se esperaba un identificador después de 'if'");

  expect(TokenType::EQUAL, "Se esperaba '=' en la condición");

  std::string conditionValue = peek().value;
  expect(TokenType::VALUE, "Se esperaba un valor de comparación");

  while (!check(TokenType::ENDIF) && !isAtEnd())
  {

#ifdef DEBUG
    std::cout << "[BODY] Procesando comando: " << peek().value << std::endl;
#endif

    advance();

    if (cursor >= tokens.size())
    {
      std::cerr << "Error: Se llegó al final del archivo sin encontrar 'endif'" << std::endl;
      exit(1);
    }
  }

  expect(TokenType::ENDIF, "Se esperaba 'endif' para cerrar el bloque");
}

/**
 * PARSE LOOP
 */
void ParseLoop::parse()
{
  expect(TokenType::LOOP, "Se esperaba la palabra clave 'loop'");

  // std::string iterations = peek().value;
  expect(TokenType::NUMBER, "Se esperaba un número de iteraciones después de 'loop'");

  while (!check(TokenType::DLOOP) && !isAtEnd())
  {
    Token t = advance();
#ifdef DEBUG
    std::cout << "  [Loop Body] Saltando token: " << t.value << std::endl;
#endif
  }

  expect(TokenType::DLOOP, "Se esperaba 'dloop' para cerrar el bucle");
}
