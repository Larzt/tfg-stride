#pragma once
#include "esp_err.h"
#include "esp_log.h"
#include <stdint.h>

void i2c_scan();
esp_err_t pcf8574_init(uint8_t i2c_addr);
esp_err_t pcf8574_led_init(uint8_t pin);
esp_err_t pcf8574_led_on(uint8_t pin);
esp_err_t pcf8574_led_off(uint8_t pin);
esp_err_t pcf8574_led_toggle(uint8_t pin);
int pcf8574_led_get_state(uint8_t pin);
