#pragma once
#include <unordered_map>
#include <memory>
#include "output.h"
#include "input.h"

class RuntimeContext
{
private:
  std::unordered_map<int, std::unique_ptr<OutputPin>> outputs;
  std::unordered_map<int, std::unique_ptr<InputPin>> inputs;

public:
  OutputPin *get_or_create_output(int pin);
  InputPin *get_or_create_input(int pin);
};
