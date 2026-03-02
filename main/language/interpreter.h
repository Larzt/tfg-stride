#pragma once
#include <vector>
#include <unordered_map>
#include "lexer.h"
#include "output.h"
#include "input.h"

class Interpreter
{
public:
  void execute(const std::vector<Token> &tokens);

private:
  std::unordered_map<std::string, OutputPin *> outputs;
};
