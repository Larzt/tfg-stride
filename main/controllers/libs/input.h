#pragma once
#include "pin_base.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class InputPin : public PinBase
{
public:
  InputPin(gpio_num_t pin, bool pull_up = true)
      : PinBase(pin), _is_pull_up(pull_up) {}

  void init() override;

  inline int read() { return gpio_get_level(_pin); }

  bool wait_for_long_press(uint32_t timeout_ms);

private:
  bool _is_pull_up;
  bool _is_pressing;
  TickType_t _start_time;
  TickType_t _last_log_time;
};
