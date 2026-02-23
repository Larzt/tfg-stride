#include <string>
#include <vector>
#include <sstream>

enum class TokenType
{
  OUTPUT,
  INPUT,
  LED,
  BUTTON,
  PIN,
  NAME,
  WRITE,
  READ,
  NUMBER,
  VALUE,
  EQUAL,
  SPACE,
  UNKNOWN
};

struct Token
{
  TokenType type;
  std::string value;
};

std::vector<Token> Tokenize(const std::string &line)
{
  std::vector<Token> tokens;
  std::istringstream iss(line);
  std::string word;
  while (iss >> word)
  {
    if (word == "LED")
      tokens.emplace_back(TokenType::LED);
    else if (word == "OUTPUT")
      tokens.emplace_back(TokenType::OUTPUT);
    else if (word == "INPUT")
      tokens.emplace_back(TokenType::INPUT);
    else if (word == "LED")
      tokens.emplace_back(TokenType::LED);
    else if (word == "BUTTON")
      tokens.emplace_back(TokenType::BUTTON);
    else if (word == "PIN")
      tokens.emplace_back(TokenType::PIN);
    else if (word == "NAME")
      tokens.emplace_back(TokenType::NAME);
    else if (word == "WRITE")
      tokens.emplace_back(TokenType::WRITE);
    else if (word == "READ")
      tokens.emplace_back(TokenType::READ);
    else if (word == "NUMBER")
      tokens.emplace_back(TokenType::NUMBER);
    else if (word == "VALUE")
      tokens.emplace_back(TokenType::VALUE);
    else if (word == "EQUAL")
      tokens.emplace_back(TokenType::EQUAL);
    else if (word == "SPACE")
      tokens.emplace_back(TokenType::SPACE);
    else
      tokens.emplace_back(TokenType::UNKNOWN);
  }
  return tokens;
}
