#include <iostream>
#include <cassert>
#include "lexer.h"
#include "check.h"

// g++ -std=c++17 tests/lexer_test.cc language/lexer.cc -Ilanguage -Iutils -o lexer_test

void _1testTokenizer()
{
  std::cout << RESET << "Test 1: " << std::endl;

  std::string line = "output=led name=myLed pin=16";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(9);
  check(tokens.size(), expectedSize, "tokens.size()");
  check(tokens[0].type, TokenType::OUTPUT, "tokens[0].type");
  check(tokens[0].value, std::string("output"), "tokens[0].value");
  check(tokens[2].type, TokenType::LED, "tokens[2].type");
  check(tokens[2].value, std::string("led"), "tokens[2].value");
}

void _2testTokenizer()
{
  std::cout << RESET << "Test 2: " << std::endl;
  std::string line = "write=myLed on";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(4);
  check(tokens.size(), expectedSize, "tokens.size()");
  check(tokens[0].type, TokenType::WRITE, "tokens[0].type");
  check(tokens[0].value, std::string("write"), "tokens[0].value");
  check(tokens[2].type, TokenType::IDENTIFIER, "tokens[2].type");
  check(tokens[2].value, std::string("myLed"), "tokens[2].value");
  check(tokens[3].type, TokenType::VALUE, "tokens[3].type");
  check(tokens[3].value, std::string("on"), "tokens[3].value");
}

void _3testTokenizer()
{
  std::cout << RESET << "Test 3: " << std::endl;

  std::string line = "output=led name=myLed pin=16\n\
      write=myLed ON\n\
      read = myLed\n\
      write = myLed OFF";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(20);
  check(tokens.size(), expectedSize, "tokens.size()");
  check(tokens[9].type, TokenType::WRITE, "tokens[9].type");
  check(tokens[9].value, std::string("write"), "tokens[9].value");
  check(tokens[13].type, TokenType::READ, "tokens[13].type");
  check(tokens[13].value, std::string("read"), "tokens[13].value");
  check(tokens[15].type, TokenType::IDENTIFIER, "tokens[15].type");
  check(tokens[15].value, std::string("myLed"), "tokens[15].value");
  check(tokens[16].type, TokenType::WRITE, "tokens[14].type");
  check(tokens[16].value, std::string("write"), "tokens[14].value");
}

void _4testTokenizer()
{
  std::cout << RESET << "Test 3: " << std::endl;

  std::string line = "i2c read 0x76 0xFA 3 -> temp";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(7);
  check(tokens.size(), expectedSize, "tokens.size()");
  check(tokens[0].type, TokenType::I2C, "tokens[0].type");
  check(tokens[0].value, std::string("i2c"), "tokens[0].value");
  check(tokens[1].type, TokenType::READ, "tokens[1].type");
  check(tokens[1].value, std::string("read"), "tokens[1].value");
  check(tokens[2].type, TokenType::HEX_NUMBER, "tokens[2].type");
  check(tokens[2].value, std::string("0x76"), "tokens[2].value");
  check(tokens[3].type, TokenType::HEX_NUMBER, "tokens[3].type");
  check(tokens[3].value, std::string("0xFA"), "tokens[3].value");
  check(tokens[4].type, TokenType::NUMBER, "tokens[4].type");
  check(tokens[4].value, std::string("3"), "tokens[4].value");
  check(tokens[5].type, TokenType::ARROW, "tokens[5].type");
  check(tokens[5].value, std::string("->"), "tokens[5].value");
  check(tokens[6].type, TokenType::IDENTIFIER, "tokens[6].type");
  check(tokens[6].value, std::string("temp"), "tokens[6].value");
}

int main()
{
  _1testTokenizer();
  _2testTokenizer();
  _3testTokenizer();
  _4testTokenizer();

  std::cout << RESET << passed << " PASSED" << std::endl;
  std::cout << failed << " FAILED" << std::endl;

  return failed > 0 ? 1 : 0;
  return 0;
}
