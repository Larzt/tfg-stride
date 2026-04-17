#include <iostream>
#include <cassert>
#include "lexer.h"
#include "line_lexer.h"
#include "check.h"

// g++ -std=c++17 tests/lexer_test.cc language/lexer.cc -Ilanguage -Iutils -o lexer_test -D TEST

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

void _5testArrowAssignment()
{
  std::cout << RESET << "Test 5: Arrow assignment (value -> var)" << std::endl;

  std::string line = "3 -> t";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(3);
  check(tokens.size(), expectedSize, "tokens.size()");

  check(tokens[0].type, TokenType::NUMBER, "tokens[0].type");
  check(tokens[0].value, std::string("3"), "tokens[0].value");

  check(tokens[1].type, TokenType::ARROW, "tokens[1].type");
  check(tokens[1].value, std::string("->"), "tokens[1].value");

  check(tokens[2].type, TokenType::IDENTIFIER, "tokens[2].type");
  check(tokens[2].value, std::string("t"), "tokens[2].value");
}

void _6testEqualAssignment()
{
  std::cout << RESET << "Test 6: Equal assignment (var = value)" << std::endl;

  std::string line = "T_RAW=3";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(3);
  check(tokens.size(), expectedSize, "tokens.size()");

  check(tokens[0].type, TokenType::IDENTIFIER, "tokens[0].type");
  check(tokens[0].value, std::string("T_RAW"), "tokens[0].value");

  check(tokens[1].type, TokenType::ASSIGN, "tokens[1].type");
  check(tokens[1].value, std::string("="), "tokens[1].value");

  check(tokens[2].type, TokenType::NUMBER, "tokens[2].type");
  check(tokens[2].value, std::string("3"), "tokens[2].value");
}

void _7testEqualSyntax()
{
  std::cout << RESET << "Test 7: Invalid syntax" << std::endl;
  std::string line = "if myLed == on\n\
      write=myLed off\n\
      endif";
  auto tokens = Tokenize(line);
  auto expectedSize = static_cast<std::vector<Token>::size_type>(9);
  check(tokens.size(), expectedSize, "tokens.size()");
  check(tokens[0].type, TokenType::IF, "tokens[0].type");
  check(tokens[0].value, std::string("if"), "tokens[0].value");
  check(tokens[1].type, TokenType::IDENTIFIER, "tokens[1].type");
  check(tokens[1].value, std::string("myLed"), "tokens[1].value");
  check(tokens[2].type, TokenType::IS_EQUAL, "tokens[2].type");
  check(tokens[2].value, std::string("=="), "tokens[2].value");
  check(tokens[3].type, TokenType::VALUE, "tokens[3].type");
  check(tokens[3].value, std::string("on"), "tokens[3].value");
}

void _8testLessOperators()
{
  std::cout << RESET << "Test 8: Less than and Less equal (<, <=)" << std::endl;
  std::string line = "a < 5 \n b <= 10";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(6);
  check(tokens.size(), expectedSize, "tokens.size()");

  check(tokens[1].type, TokenType::LESS_THAN, "tokens[1].type");
  check(tokens[1].value, std::string("<"), "tokens[1].value");

  check(tokens[4].type, TokenType::LESS_EQUAL, "tokens[4].type");
  check(tokens[4].value, std::string("<="), "tokens[4].value");
}

void _9testGreaterOperators()
{
  std::cout << RESET << "Test 9: Greater than and Greater equal (>, >=)" << std::endl;
  std::string line = "x > 0 \n y >= x";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(6);
  check(tokens.size(), expectedSize, "tokens.size()");

  check(tokens[1].type, TokenType::GREATER_THAN, "tokens[1].type");
  check(tokens[1].value, std::string(">"), "tokens[1].value");

  check(tokens[4].type, TokenType::GREATER_EQUAL, "tokens[4].type");
  check(tokens[4].value, std::string(">="), "tokens[4].value");
}

void _10testNotEqualOperator()
{
  std::cout << RESET << "Test 10: Not equal (!=)" << std::endl;
  std::string line = "if state != on";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(4);
  check(tokens.size(), expectedSize, "tokens.size()");

  check(tokens[2].type, TokenType::NOT_EQUAL, "tokens[2].type");
  check(tokens[2].value, std::string("!="), "tokens[2].value");
}

void _11testMixedAssignAndCompare()
{
  std::cout << RESET << "Test 11: Mix Assign (=) and Is Equal (==)" << std::endl;
  std::string line = "target = 10 \n if target == 10";

  auto tokens = Tokenize(line);

  auto expectedSize = static_cast<std::vector<Token>::size_type>(7);
  check(tokens.size(), expectedSize, "tokens.size()");

  check(tokens[1].type, TokenType::ASSIGN, "tokens[1].type");
  check(tokens[1].value, std::string("="), "tokens[1].value");

  // Verificamos Comparación '=='
  check(tokens[5].type, TokenType::IS_EQUAL, "tokens[5].type");
  check(tokens[5].value, std::string("=="), "tokens[5].value");
}

int main()
{
  _1testTokenizer();
  _2testTokenizer();
  _3testTokenizer();
  _4testTokenizer();
  _5testArrowAssignment();
  _6testEqualAssignment();

  _7testEqualSyntax();
  _8testLessOperators();
  _9testGreaterOperators();
  _10testNotEqualOperator();
  _11testMixedAssignAndCompare();

  std::cout << RESET << passed << " PASSED" << std::endl;
  std::cout << failed << " FAILED" << std::endl;

  return failed > 0 ? 1 : 0;
}
