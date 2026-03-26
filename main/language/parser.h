#pragma once

#include <vector>
#include "lexer.h"

class BaseParser
{
public:
  explicit BaseParser(const std::vector<Token> &tokens, size_t &currentPos)
      : tokens(tokens), cursor(currentPos) {}

  // Devuelve el token actual sin avanzar
  inline Token peek() const { return tokens[cursor]; }
  // Avanza y devuelve el token
  inline Token advance() { return tokens[cursor++]; }

  inline bool isAtEnd() const { return cursor >= tokens.size(); }
  bool check(TokenType type) const;
  void expect(TokenType type, std::string message);

  virtual ~BaseParser() = default;
  virtual void parse() = 0;

protected:
  const std::vector<Token> &tokens;
  size_t &cursor;
};

class ParseIf : public BaseParser
{
public:
  using BaseParser::BaseParser;
  void parse() override;
};

class ParseLoop : public BaseParser
{
public:
  using BaseParser::BaseParser;
  void parse() override;
};
