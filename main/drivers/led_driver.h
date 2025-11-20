#pragma once

#include <stdint.h>

// Inicializa el LED
void led_init(int pin);

// Cambia el estado del LED (on/off)
void led_toggle(void);

// Devuelve el estado actual del LED
int led_get_state(void);
