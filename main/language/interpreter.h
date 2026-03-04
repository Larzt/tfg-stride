#pragma once
#include <vector>
#include <map>
#include "lexer.h"
#include "output.h"
#include "input.h"

class Interpreter
{
public:
  void execute(const std::vector<std::vector<Token>> &tokens);

private:
  void executeSingle(const std::vector<Token> &tokens);
  void executeOutput(const std::vector<Token> &tokens);
  void executeWrite(const std::vector<Token> &tokens);
  void executeWait(const std::vector<Token> &tokens);
  size_t executeLoop(const std::vector<std::vector<Token>> &program, size_t currentIndex);

  std::map<std::string, OutputPin *> outputs;
};
