#pragma once
#include "driver/gpio.h"

class PinBase
{
protected:
  gpio_num_t _pin;

public:
  PinBase(gpio_num_t pin) : _pin(pin) {}
  virtual ~PinBase() = default;
  virtual void init() = 0;

  gpio_num_t get_pin() const { return _pin; }
};
