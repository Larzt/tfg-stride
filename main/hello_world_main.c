/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define PIN_LED 2

void app_main(void)
{
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
    printf("Iniciando EPS32...\n");

    static uint8_t out_state = 0;

    while (true)
    {
        out_state = !out_state;

        if (out_state)
        {
            printf("_ON\n");
        }
        else
        {
            printf("_OFF\n");
        }

        gpio_set_level(PIN_LED, out_state);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
