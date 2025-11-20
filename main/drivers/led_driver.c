#include "led_driver.h"
#include "driver/gpio.h"

static int LED_PIN = -1;
static uint8_t led_state = 0;

void led_init(int pin)
{
  LED_PIN = pin;

  gpio_config_t io_conf = {
      .pin_bit_mask = (1ULL << LED_PIN),
      .mode = GPIO_MODE_INPUT_OUTPUT};

  gpio_config(&io_conf);

  gpio_set_level(LED_PIN, 0);
}

void led_toggle(void)
{
  led_state ^= 1;
  gpio_set_level(LED_PIN, led_state);
}

int led_get_state(void)
{
  return led_state;
}
