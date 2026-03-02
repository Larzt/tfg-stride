#include "runtime.h"

OutputPin *RuntimeContext::get_or_create_output(int pin)
{
  if (outputs.find(pin) == outputs.end())
  {
    outputs[pin] = std::make_unique<OutputPin>((gpio_num_t)pin);
    outputs[pin]->init();
  }
  return outputs[pin].get();
}

InputPin *RuntimeContext::get_or_create_input(int pin)
{
  if (inputs.find(pin) == inputs.end())
  {
    inputs[pin] = std::make_unique<InputPin>((gpio_num_t)pin, false);
    inputs[pin]->init();
  }
  return inputs[pin].get();
}
