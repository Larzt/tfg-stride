#include "input.h"

#include "esp_log.h"

void InputPin::init()
{
  gpio_reset_pin(_pin);
  gpio_set_direction(_pin, GPIO_MODE_INPUT);
  if (_is_pull_up)
  {
    gpio_set_pull_mode(_pin, GPIO_PULLUP_ONLY);
  }
  else
  {
    gpio_set_pull_mode(_pin, GPIO_PULLDOWN_ONLY);
  }
}

bool InputPin::wait_for_long_press(uint32_t timeout_ms)
{
  TickType_t now = xTaskGetTickCount();
  int current_state = read();

  if (current_state == 1 && !_is_pressing)
  {
    _is_pressing = true;
    _start_time = xTaskGetTickCount();
  }

  if (_is_pressing)
  {
    uint32_t duration_ms = (now - _start_time) * portTICK_PERIOD_MS;
    if (duration_ms >= timeout_ms)
    {
      _is_pressing = false;
      return true;
    }
  }

  return false;
}
