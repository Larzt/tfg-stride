#pragma once
#include "pin_base.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class OutputPin : public PinBase
{
public:
  using PinBase::PinBase;

  void init() override;

  void toggle();
  inline int get_level() { return _state; }
  void turn(bool state);

private:
  inline void set_level(int level) { gpio_set_level(_pin, level); }

  bool _is_pull_up;
  int _state;
};
