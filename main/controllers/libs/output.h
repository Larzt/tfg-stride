#pragma once
#include "pin_base.h"
#include "driver/gpio.h"

class OutputPin : public PinBase
{
public:
  OutputPin(gpio_num_t pin, bool active_low = false)
      : PinBase(pin), _active_low(active_low), _state(false) {}

  void init() override;

  void on();
  void off();
  void toggle();
  void set(bool state); // true = ON, false = OFF
  bool get() const { return _state; }

private:
  void apply();

  bool _active_low; // true si el dispositivo es activo en bajo
  bool _state;      // estado lógico real (ON/OFF)
};
