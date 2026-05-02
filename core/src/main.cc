
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#include "stride_logger.h"


extern "C" void app_main(void)
{
    gpio_num_t pin = GPIO_NUM_2;
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);

    StrideLogger::Log(StrideSubsystem::None, "Iniciando el ESP32");

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

        gpio_set_level(pin, out_state);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
