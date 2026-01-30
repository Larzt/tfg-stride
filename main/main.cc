#include "WifiHandler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

extern "C" void app_main(void)
{
    WifiHandler wifi;

    wifi.init("MI_ESP32_AP", "123456789",
              "MOVISTAR_478F", "5RibhXQvhsQkqFiCizyC");

    // Core 1
    xTaskCreatePinnedToCore([](void *pvParameters)
                            {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(5000));
        } }, "MainTask", 4096, NULL, 5, NULL, 1);
}
