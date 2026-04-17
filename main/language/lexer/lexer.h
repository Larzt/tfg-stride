#pragma once

#include <string>
#include <vector>
#include <iostream>

enum class TokenType
{
  // BASICS
  FILE, OUTPUT, INPUT, PIN, BUTTON, LED, NAME, WRITE, READ, PRINT,

  // FLOW CONTROL
  IF, ELSE, ENDIF, LOOP, DLOOP, WAIT,

  // I2C
  I2C, INIT, SDA, SCL,

  // LITERALS
  STRING, NUMBER, HEX_NUMBER, IDENTIFIER, VALUE,

  // SYMBOLS
  ARROW, ASSIGN,

  // CONDITIONALS
  IS_EQUAL,        // ==
  NOT_EQUAL,       // !=
  LESS_EQUAL,      // <=
  GREATER_EQUAL,   // >=
  LESS_THAN,       // <
  GREATER_THAN,    // >

  UNKNOWN
};

struct Token
{
  TokenType type;
  std::string value;

  Token() = default;
  Token(const Token &token) = default;
  Token(TokenType t) : type(t) {}
  Token(TokenType t, const std::string &v) : type(t), value(v) {}
};

std::vector<Token> Tokenize(const std::string &line);

std::ostream &operator<<(std::ostream &out, TokenType type);
std::ostream &operator<<(std::ostream &out, const Token& token);
