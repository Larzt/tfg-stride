#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <algorithm>

enum class TokenType
{
  // BASICS
  FILE,
  OUTPUT,
  INPUT,
  PIN,
  BUTTON,
  LED,
  NAME,
  WRITE,
  READ,
  PRINT,

  // FLOW CONTROL
  IF,
  ELSE,
  ENDIF,
  LOOP,
  DLOOP,
  WAIT,

  // I2C
  I2C,
  INIT,
  SDA,
  SCL,

  // LITERALS
  NUMBER,
  HEX_NUMBER,
  IDENTIFIER, // Variables
  VALUE,

  // SYMBOLS
  ARROW,
  EQUAL,

  UNKNOWN
};

static const std::unordered_map<std::string, TokenType> keywords = {
    {"file", TokenType::FILE},
    {"output", TokenType::OUTPUT},
    {"input", TokenType::INPUT},
    {"led", TokenType::LED},
    {"button", TokenType::BUTTON},
    {"pin", TokenType::PIN},
    {"name", TokenType::NAME},
    {"write", TokenType::WRITE},
    {"read", TokenType::READ},
    {"print", TokenType::PRINT},

    // FLOW CONTROL
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"endif", TokenType::ENDIF},
    {"loop", TokenType::LOOP},
    {"dloop", TokenType::DLOOP},
    {"wait", TokenType::WAIT},

    // I2C
    {"i2c", TokenType::I2C},
    {"init", TokenType::INIT},
    {"sda", TokenType::SDA},
    {"scl", TokenType::SCL},

    // OTHERS
    {"->", TokenType::ARROW},
    {"=", TokenType::EQUAL},
    {"on", TokenType::VALUE},
    {"off", TokenType::VALUE}};

struct Token
{
  TokenType type;
  std::string value;

  Token() = default;

  Token(const Token &token) : type(token.type), value(token.value) {}
  Token(TokenType t) : type(t) {}
  Token(TokenType t, const std::string &v) : type(t), value(v) {}
};

Token GenerateToken(const std::string &word);
std::vector<Token> Tokenize(const std::string &line);
std::ostream &operator<<(std::ostream &out, TokenType type);
std::ostream &operator<<(std::ostream &out, Token token);
