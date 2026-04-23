#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "lexer.h"
#include "line_lexer.h"
#include "check.h"
#include "utility.h"

// -----------------------------------------------------
// Happy Path
// -----------------------------------------------------
void testKeywordTokens(TestRunner &runner)
{
  runner.setTest("Test Keywords");

  auto tokens = Tokenize("output write read if endif");

  checkToken(runner, tokens[0], TokenType::OUTPUT, "output", "output");
  checkToken(runner, tokens[1], TokenType::WRITE, "write", "write");
  checkToken(runner, tokens[2], TokenType::READ, "read", "read");
  checkToken(runner, tokens[3], TokenType::IF, "if", "if");
  checkToken(runner, tokens[4], TokenType::ENDIF, "endif", "endif");
}

void testIdentifierTokens(TestRunner &runner)
{
  runner.setTest("Test Identifiers");

  auto tokens = Tokenize("myLed temp sensor_1");

  checkToken(runner, tokens[0], TokenType::IDENTIFIER, "myLed", "identifier 1");
  checkToken(runner, tokens[1], TokenType::IDENTIFIER, "temp", "identifier 2");
  checkToken(runner, tokens[2], TokenType::IDENTIFIER, "sensor_1", "identifier 3");
}

void testNumberTokens(TestRunner &runner)
{
  runner.setTest("Test Numbers");

  auto tokens = Tokenize("123 0xFF 42");

  checkToken(runner, tokens[0], TokenType::NUMBER, "123", "number 1");
  checkToken(runner, tokens[1], TokenType::HEX_NUMBER, "0xFF", "hex number");
  checkToken(runner, tokens[2], TokenType::NUMBER, "42", "number 2");
}

void testAssignmentTokens(TestRunner &runner)
{
  runner.setTest("Test Assignment");

  auto tokens = Tokenize("a = 5  3 -> b");

  checkToken(runner, tokens[0], TokenType::IDENTIFIER, "a", "identifier");
  checkToken(runner, tokens[1], TokenType::ASSIGN, "=", "assign");
  checkToken(runner, tokens[2], TokenType::NUMBER, "5", "value");

  checkToken(runner, tokens[3], TokenType::NUMBER, "3", "value 2");
  checkToken(runner, tokens[4], TokenType::ARROW, "->", "arrow");
  checkToken(runner, tokens[5], TokenType::IDENTIFIER, "b", "target");
}

void testComparisonTokens(TestRunner &runner)
{
  runner.setTest("Test Comparisons");

  auto tokens = Tokenize("a == b a != b a < b a <= b a > b a >= b");

  checkToken(runner, tokens[1], TokenType::IS_EQUAL, "==", "==");
  checkToken(runner, tokens[4], TokenType::NOT_EQUAL, "!=", "!=");
  checkToken(runner, tokens[7], TokenType::LESS_THAN, "<", "<");
  checkToken(runner, tokens[10], TokenType::LESS_EQUAL, "<=", "<=");
  checkToken(runner, tokens[13], TokenType::GREATER_THAN, ">", ">");
  checkToken(runner, tokens[16], TokenType::GREATER_EQUAL, ">=", ">=");
}

void testValueTokens(TestRunner &runner)
{
  runner.setTest("Test Values");

  auto tokens = Tokenize("on OFF");

  checkToken(runner, tokens[0], TokenType::VALUE, "on", "value on");
  checkToken(runner, tokens[1], TokenType::VALUE, "OFF", "value off");
}

void testPrintTokens(TestRunner &runner)
{
  runner.setTest("Test Print");

  auto tokens = Tokenize("print \"hola mundo\"");

  checkToken(runner, tokens[0], TokenType::PRINT, "print", "print");
  checkToken(runner, tokens[1], TokenType::STRING, "hola mundo", "string message");
}

void testI2CTokens(TestRunner &runner)
{
  runner.setTest("Test I2C");

  auto tokens = Tokenize("i2c read 0x76 0xFA 3 -> temp");

  checkToken(runner, tokens[0], TokenType::I2C, "i2c", "i2c");
  checkToken(runner, tokens[1], TokenType::READ, "read", "read");
  checkToken(runner, tokens[2], TokenType::HEX_NUMBER, "0x76", "addr");
  checkToken(runner, tokens[3], TokenType::HEX_NUMBER, "0xFA", "register");
  checkToken(runner, tokens[4], TokenType::NUMBER, "3", "length");
  checkToken(runner, tokens[5], TokenType::ARROW, "->", "arrow");
  checkToken(runner, tokens[6], TokenType::IDENTIFIER, "temp", "target");
}

void testProgramScenarios(TestRunner &runner)
{
  runner.setTest("Test Program Scenarios");

  std::string line = "output=led name=myLed pin=16\n\
write=myLed ON\n\
if myLed == on\n\
write=myLed OFF\n\
endif\n\
print myLed";

  auto tokens = Tokenize(line);

  check(runner, tokens.size() > 0, true, "non empty");

  checkToken(runner, tokens[0], TokenType::OUTPUT, "output", "first token");
  checkToken(runner, tokens[2], TokenType::LED, "led", "led");

  checkToken(runner, tokens[9], TokenType::WRITE, "write", "write");
  checkToken(runner, tokens[13], TokenType::IF, "if", "if");

  checkToken(runner, tokens[15], TokenType::IS_EQUAL, "==", "==");
  checkToken(runner, tokens[21], TokenType::ENDIF, "endif", "endif");

  checkToken(runner, tokens[22], TokenType::PRINT, "print", "print");
  checkToken(runner, tokens.back(), TokenType::VALUE, "off", "off string");
}

// -----------------------------------------------------
// Error path
// -----------------------------------------------------
void testInvalidTokens(TestRunner &runner)
{
  runner.setTest("Test Invalid Tokens");

  auto tokens = Tokenize("@ @@ ###");

  check(runner, tokens.size() > 0, true, "tokens generated");

  for (const auto &t : tokens)
  {
    check(runner, t.type, TokenType::UNKNOWN, "invalid token detected");
  }
}

void testInvalidHex(TestRunner &runner)
{
  runner.setTest("Test Invalid Hex");

  auto tokens = Tokenize("0x 0xG1 0xZZ");
  check(runner, tokens.size() > 0, true, "tokens exist");
  for (const auto &t : tokens)
  {
    check(runner, t.type == TokenType::HEX_NUMBER, false, "invalid hex should not be valid");
  }
}

void testUnclosedString(TestRunner &runner)
{
  runner.setTest("Test Unclosed String");

  auto tokens = Tokenize("print \"hola");
  check(runner, tokens.size() > 0, true, "tokens exist");
  check(runner, tokens[1].type == TokenType::STRING, false, "string should be invalid");
}

void testInvalidOperators(TestRunner &runner)
{
  runner.setTest("Test Invalid Operators");

  auto tokens = Tokenize("a === b a <> b a => b");
  for (const auto &t : tokens)
  {
    check(runner, t.type == TokenType::IS_EQUAL, false, "invalid operator should fail");
  }
}

void testBrokenAssignments(TestRunner &runner)
{
  runner.setTest("Test Broken Assignments");
  auto tokens = Tokenize("= 5");

  check(runner, tokens[0].type, TokenType::ASSIGN, "Token = detectado");
  check(runner, tokens[1].type, TokenType::NUMBER, "Token 5 detectado");

  bool isValidSequence = (tokens.size() >= 3);
  check(runner, isValidSequence, false, "La secuencia es demasiado corta para ser una asignación");
}

void testInvalidKeywords(TestRunner &runner)
{
  runner.setTest("Test Invalid Keywords");
  auto tokens = Tokenize("outpu writ reaad iff endiff");
  for (const auto &t : tokens)
  {
    check(runner, t.type == TokenType::OUTPUT, false, "typo keyword");
    check(runner, t.type == TokenType::WRITE, false, "typo keyword");
  }
}

void testWeirdSpacing(TestRunner &runner)
{
  runner.setTest("Test Weird Spacing");
  auto tokens = Tokenize("a=5    b   =    6");
  check(runner, tokens.size() > 0, true, "tokens exist");
  checkToken(runner, tokens[0], TokenType::IDENTIFIER, "a", "a");
  checkToken(runner, tokens[1], TokenType::ASSIGN, "=", "=");
  checkToken(runner, tokens[2], TokenType::NUMBER, "5", "5");
}

void testEmptyInput(TestRunner &runner)
{
  runner.setTest("Test Empty Input");
  auto tokens = Tokenize("");
  check(runner, tokens.size(), static_cast<size_t>(0), "empty input");
}

void testOnlyGarbage(TestRunner &runner)
{
  runner.setTest("Test Only Garbage");
  auto tokens = Tokenize("$$$$$");
  for (const auto &t : tokens)
  {
    check(runner, t.type, TokenType::UNKNOWN, "garbage token");
  }
}

int main()
{
  TestRunner runner;

  // Happy Path
  testKeywordTokens(runner);
  testIdentifierTokens(runner);
  testNumberTokens(runner);
  testAssignmentTokens(runner);
  testComparisonTokens(runner);
  testPrintTokens(runner);
  testI2CTokens(runner);
  testProgramScenarios(runner);

  // Error Path
  testUnclosedString(runner);
  testInvalidTokens(runner);
  testInvalidHex(runner);
  testInvalidOperators(runner);
  testBrokenAssignments(runner);
  testInvalidKeywords(runner);
  testWeirdSpacing(runner);
  testEmptyInput(runner);
  testOnlyGarbage(runner);

  std::cout << "PASSED: " << runner.passed << kENDL;
  std::cout << "FAILED: " << runner.failed << kENDL;

  if (!runner.failedMessages.empty())
  {
    std::cout << kENDL << RED << "FAILED TESTS:" << RESET << kENDL;
    for (const auto &msg : runner.failedMessages)
    {
      std::cout << RED << msg << RESET << kENDL;
    }
  }

  return runner.allPassed() ? 0 : 1;
}
