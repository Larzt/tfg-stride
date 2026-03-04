#pragma once

#include "pin_base.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class InputPin : public PinBase
{
public:
  explicit InputPin(gpio_num_t pin, bool pull_up = true);

  void init() override;

  bool is_pressed();                     // Pulsación simple (con debounce)
  bool wait_for_long_press(uint32_t ms); // Long press

private:
  bool read_raw();       // Lectura directa GPIO
  bool read_debounced(); // Lectura con antirrebote

private:
  bool _pull_up;             // true = pull-up interno
  bool _is_pressing = false; // estado interno long press
  bool _last_state = false;  // último estado estable
  bool _last_reading = false;

  TickType_t _start_time = 0;
  TickType_t _last_log_time = 0;
  TickType_t _last_debounce_time = 0;

  static constexpr uint32_t DEBOUNCE_MS = 30;
};
