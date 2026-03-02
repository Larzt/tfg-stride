#include "output.h"
#include "esp_log.h"

void OutputPin::init()
{
  gpio_reset_pin(_pin);
  gpio_set_direction(_pin, GPIO_MODE_OUTPUT);
  this->turn(false);
  ESP_LOGW("OutPIN: ", "Estado del pin:%d inicializado: %d", get_pin(), _state);
}

void OutputPin::toggle()
{
  _state = !_state;
  set_level(_state);
  ESP_LOGW("OutPIN: ", "Estado del pin:%d ha cambiado: %d", get_pin(), _state);
}

void OutputPin::turn(bool state)
{
  // false = on
  // true = off
  _state = !state;
  set_level(_state);
  ESP_LOGW("OutPIN: ", "Estado del pin:%d ha cambiado: %d", get_pin(), _state);
}
