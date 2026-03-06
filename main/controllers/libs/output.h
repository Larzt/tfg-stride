#pragma once
#include "pin_base.h"
#include "driver/gpio.h"

class OutputPin : public PinBase
{
public:
  OutputPin(gpio_num_t pin, bool state = false, bool active_low = false)
      : PinBase(pin), _state(state), _active_low(active_low) {}

  void init() override;

  void on();
  void off();
  void toggle();
  void set(bool state); // true = ON, false = OFF
  bool get() const { return _state; }

private:
  void apply();

  bool _state;      // estado lógico real (ON/OFF)
  bool _active_low; // true si el dispositivo es activo en bajo
};
