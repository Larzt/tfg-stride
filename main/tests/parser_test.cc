#include <iostream>
#include <vector>
#include "utility.h"
#include "lexer.h"
#include "parser.h"
#include "check.h"

// g++ -std=c++17 tests/parser_test.cc language/lexer.cc language/parser.cc -Iconsts -Ilanguage -Iutils -o parser_test -D TEST

void _testParseIf(TestRunner& runner)
{
  size_t index = 0;
  std::string code = "if myLed = on\n\
  write=myLed off\n\
  endif";

  std::vector<Token> tokens = Tokenize(code);

  ParseIf parser(tokens, index);
  parser.parse();

  check(runner, index, (size_t)9, "Parse If");
}

void _testParseElseIF(TestRunner& runner)
{

  size_t index = 0;
  std::string code = "if myLed = on\n\
  write=myLed off\n\
  else\n\
  write=myLed on\n\
  endif";

  std::vector<Token> tokens = Tokenize(code);

  ParseIf parser(tokens, index);
  parser.parse();
  check(runner, index, (size_t)14, "Parse If Else");
}

void _testParseLoop(TestRunner& runner)
{
  size_t index = 0;
  std::string code = "loop 5\n\
  write=myLed off\n\
  wait 2\n\
  write=myLed on\n\
  dloop";

  std::vector<Token> tokens = Tokenize(code);

  ParseLoop parser(tokens, index);
  parser.parse();
  check(runner, index, (size_t)13, "Parse Loop");
}

int main()
{
  TestRunner runner;

  _testParseIf(runner);
  _testParseElseIF(runner);
  _testParseLoop(runner);

  std::cout << CYAN << "PASSED: " << runner.passed << kENDL;
  std::cout << CYAN << "FAILED: " << runner.failed << kENDL;


  if (!runner.failedMessages.empty())
  {
    std::cout << kENDL << RED << "FAILED TESTS:" << RESET << kENDL;
    for (const auto &msg : runner.failedMessages)
    {
      std::cout << RED << msg << RESET << kENDL;
    }
  }

  return runner.failed ? 1 : 0;
}
