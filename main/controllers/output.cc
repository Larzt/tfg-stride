#include "output.h"

void OutputPin::init()
{
  gpio_reset_pin(_pin);
  gpio_set_direction(_pin, GPIO_MODE_OUTPUT);
  this->turn(false);
}

void OutputPin::toggle()
{
  _state = !_state;
  set_level(_state);
}
