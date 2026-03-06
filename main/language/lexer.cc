#include "lexer.h"
#include "esp_log.h"

Token GenerateToken(const std::string &word)
{
  std::string lower = word;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  auto it = keywords.find(lower);
  if (it != keywords.end())
  {
    return Token(it->second, word);
  }

  // HEX NUMBER (0x76)
  if (word.size() > 2 && word[0] == '0' &&
      (word[1] == 'x' || word[1] == 'X'))
  {
    return Token(TokenType::HEX_NUMBER, word);
  }

  // DECIMAL NUMBER
  if (!word.empty() &&
      std::all_of(word.begin(), word.end(), ::isdigit))
  {
    return Token(TokenType::NUMBER, word);
  }

  return Token(TokenType::IDENTIFIER, word);
}

std::vector<Token> Tokenize(const std::string &line)
{
  std::vector<Token> tokens;
  std::string word = "";

  for (size_t i = 0; i < line.size(); i++)
  {

    char letter = line[i];

    // Arrow assign '->' detectado
    if (letter == '-' && i + 1 < line.size() && line[i + 1] == '>')
    {
      // Si hay algo acumulado antes, tokenizarlo
      if (!word.empty())
      {
        tokens.emplace_back(GenerateToken(word));
        word.clear();
      }

      tokens.emplace_back(TokenType::ARROW, "->");
      i++; // saltar el '>'
    }
    else if (letter == '=')
    {
      // Si hay algo acumulado antes, tokenizarlo
      if (!word.empty())
      {
        tokens.emplace_back(GenerateToken(word));
        word.clear();
      }
      tokens.emplace_back(TokenType::EQUAL, "=");
    }
    else if (isspace(letter))
    {
      if (!word.empty())
      {
        tokens.emplace_back(GenerateToken(word));
        word.clear();
      }
    }
    else
    {
      word += letter;
    }
  }

  // Token final si queda algo
  if (!word.empty())
  {
    tokens.emplace_back(GenerateToken(word));
  }

  return tokens;
}

std::ostream &operator<<(std::ostream &out, TokenType type)
{
  switch (type)
  {
  case TokenType::OUTPUT:
    out << "OUTPUT";
    break;
  case TokenType::INPUT:
    out << "INPUT";
    break;
  case TokenType::LED:
    out << "LED";
    break;
  case TokenType::BUTTON:
    out << "BUTTON";
    break;
  case TokenType::PIN:
    out << "PIN";
    break;
  case TokenType::NAME:
    out << "NAME";
    break;
  case TokenType::WRITE:
    out << "WRITE";
    break;
  case TokenType::READ:
    out << "READ";
    break;
  case TokenType::NUMBER:
    out << "NUMBER";
    break;
  case TokenType::IDENTIFIER:
    out << "IDENTIFIER";
    break;
  case TokenType::VALUE:
    out << "VALUE";
    break;

    // FLOW CONTROL
  case TokenType::IF:
    out << "IF";
    break;
  case TokenType::ELSE:
    out << "ELSE";
    break;
  case TokenType::ENDIF:
    out << "ENDIF";
    break;
  case TokenType::LOOP:
    out << "LOOP";
    break;
  case TokenType::DLOOP:
    out << "DLOOP";
    break;
  case TokenType::WAIT:
    out << "WAIT";
    break;

    // Assign
  case TokenType::EQUAL:
    out << "EQUAL";
    break;
  case TokenType::ARROW:
    out << "ARROW";
    break;

  // I2C
  case TokenType::I2C:
    out << "I2C";
    break;
  case TokenType::INIT:
    out << "INIT";
    break;
  case TokenType::SDA:
    out << "SDA";
    break;
  case TokenType::SCL:
    out << "SCL";
    break;
  case TokenType::HEX_NUMBER:
    out << "HEX_NUMBER";
    break;

  default:
    out << "UNKNOWN";
    break;
  }

  return out;
}

std::ostream &operator<<(std::ostream &out, Token token)
{
  out << token.type << ": " << token.value;
  return out;
}
