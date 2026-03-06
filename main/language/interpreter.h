#pragma once
#include <vector>
#include <map>
#include <atomic>
#include "lexer.h"
#include "output.h"
#include "input.h"

class Interpreter
{
public:
  static Interpreter &Instance()
  {
    static Interpreter instance;
    return instance;
  }

  void execute(const std::vector<std::vector<Token>> &tokens);

  inline void start_loop() { endlessLoop.store(true); }
  inline void stop_loop() { endlessLoop.store(false); }

private:
  Interpreter() = default;
  ~Interpreter() = default;
  Interpreter(const Interpreter &) = delete;
  Interpreter &operator=(const Interpreter &) = delete;

  // void executeAssign(const std::vector<Token> &tokens);
  void executeArrow(const std::vector<Token> &tokens);
  void executeSingle(const std::vector<Token> &tokens);
  void executeOutput(const std::vector<Token> &tokens);
  void executeWrite(const std::vector<Token> &tokens);
  void executeWait(const std::vector<Token> &tokens);
  size_t executeLoop(const std::vector<std::vector<Token>> &program, size_t currentIndex);
  size_t executeIf(const std::vector<std::vector<Token>> &program, size_t currentIndex);

  // !TODO: create a new class for custom variables
  std::map<std::string, int> variables;
  std::map<std::string, OutputPin *> outputs;
  std::atomic<bool> endlessLoop{true};
};
