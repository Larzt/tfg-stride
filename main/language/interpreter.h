#pragma once
#include "lexer.h"
#include "runtime.h"
#include <vector>

class Interpreter
{
private:
  RuntimeContext context;

public:
  void execute(const std::vector<Token> &tokens);
};
