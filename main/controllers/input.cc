#include "input.h"
#include "esp_log.h"

static const char *TAG = "InputPin";

InputPin::InputPin(gpio_num_t pin, bool pull_up)
    : PinBase(pin),
      _pull_up(pull_up)
{
}

void InputPin::init()
{
  gpio_reset_pin(_pin);
  gpio_set_direction(_pin, GPIO_MODE_INPUT);

  if (_pull_up)
    gpio_set_pull_mode(_pin, GPIO_PULLUP_ONLY);
  else
    gpio_set_pull_mode(_pin, GPIO_PULLDOWN_ONLY);
}

bool InputPin::read_raw()
{
  int level = gpio_get_level(_pin);

  // Si es pull-up: pulsado = LOW
  if (_pull_up)
    return (level == 0);
  else
    return (level == 1);
}

bool InputPin::read_debounced()
{
  bool reading = read_raw();
  TickType_t now = xTaskGetTickCount();

  if (reading != _last_reading)
  {
    _last_debounce_time = now;
  }

  if ((now - _last_debounce_time) > pdMS_TO_TICKS(DEBOUNCE_MS))
  {
    _last_state = reading;
  }

  _last_reading = reading;

  return _last_state;
}

bool InputPin::just_pressed()
{
    bool current_state = read_debounced();

    if (current_state && !_was_pressed)
    {
        _was_pressed = true;
        return true;
    }

    if (!current_state)
    {
        _was_pressed = false;
    }

    return false;
}

bool InputPin::is_pressed()
{
  return read_debounced();
}

bool InputPin::wait_for_long_press(uint32_t timeout_ms)
{
  TickType_t now = xTaskGetTickCount();
  bool pressed = read_debounced();

  if (pressed && !_is_pressing)
  {
    ESP_LOGI(TAG, "Pulsación iniciada");
    _is_pressing = true;
    _start_time = now;
    _last_log_time = now;
  }

  if (!pressed && _is_pressing)
  {
    ESP_LOGI(TAG, "Pulsación cancelada");
    _is_pressing = false;
  }

  if (_is_pressing)
  {
    if ((now - _last_log_time) >= pdMS_TO_TICKS(1000))
    {
      ESP_LOGI(TAG, "Mantén presionado...");
      _last_log_time = now;
    }

    uint32_t duration_ms =
        (now - _start_time) * portTICK_PERIOD_MS;

    if (duration_ms >= timeout_ms)
    {
      ESP_LOGI(TAG, "Long press detectado");
      _is_pressing = false;
      return true;
    }
  }

  return false;
}
