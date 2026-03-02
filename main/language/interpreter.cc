#include "interpreter.h"
#include <cstdlib>

void Interpreter::execute(const std::vector<Token> &tokens)
{
  if (tokens.empty())
    return;

  if (tokens[0].type == TokenType::LED)
  {
    if (tokens.size() < 3)
      return;

    int pin = std::stoi(tokens[1].value);

    OutputPin *led = context.get_or_create_output(pin);

    std::string state = tokens[2].value;

    if (state == "on")
      led->turn(true);
    else if (state == "off")
      led->turn(false);
  }

  if (tokens[0].type == TokenType::BUTTON)
  {
    if (tokens.size() < 2)
      return;

    int pin = std::stoi(tokens[1].value);
    InputPin *button = context.get_or_create_input(pin);

    if (button->read())
    {
      printf("Button pressed!\n");
    }
  }
}
