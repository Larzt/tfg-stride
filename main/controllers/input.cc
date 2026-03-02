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
    ESP_LOGI("Button", "Pulsación iniciada...");
    _is_pressing = true;
    _start_time = now;
    _last_log_time = now; // iniciar control de log
  }

  if (current_state == 0 && _is_pressing)
  {
    ESP_LOGW("Button", "Pulsacion cancelada...");
    _is_pressing = false;
  }

  if (_is_pressing)
  {
    // Log solo cada 1 segundo
    if ((now - _last_log_time) >= pdMS_TO_TICKS(1000))
    {
      ESP_LOGW("Button", "Mantén el botón presionado...");
      _last_log_time = now;
    }

    uint32_t duration_ms = (now - _start_time) * portTICK_PERIOD_MS;

    if (duration_ms >= timeout_ms)
    {
      _is_pressing = false;
      return true;
    }
  }

  return false;
}
