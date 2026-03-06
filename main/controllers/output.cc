#include "output.h"
#include "esp_log.h"

static const char *TAG = "OutputPin";

void OutputPin::init()
{
  gpio_reset_pin(_pin);
  gpio_set_direction(_pin, GPIO_MODE_OUTPUT);

  apply();

  ESP_LOGI(TAG, "Pin %d inicializado", _pin);
}

void OutputPin::apply()
{
  int level = _active_low ? !_state : _state;
  gpio_set_level(_pin, level);
  ESP_LOGI(TAG, "Pin %d aplicado, _state: %d, _active_low: %d, nivel GPIO: %d",
           _pin, _state, _active_low, gpio_get_level(_pin));
}

void OutputPin::on()
{
  _state = true;
  apply();
}

void OutputPin::off()
{
  _state = false;
  apply();
}

void OutputPin::toggle()
{
  _state = !_state;
  apply();
}

void OutputPin::set(bool state)
{
  _state = state;
  apply();
}
