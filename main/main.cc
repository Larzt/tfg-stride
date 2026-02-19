#include "WifiHandler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Server & Handlers (endpoints)
#include "server.h"

// Controllers
#include "input.h"
#include "output.h"
#include "main.h"

#define TIME_PRESSED 5000

#define MODE_PIN_26 GPIO_NUM_26
#define BUTTON_MODE_12 GPIO_NUM_12

void loop(void *pvParameters)
{
    ESP_LOGW("SYSTEM", "Tarea de entrada iniciada en Core 1");

    Server *server = (Server *)pvParameters;

    modes_manager(server);
}

void modes_manager(Server *server)
{
    InputPin mode_button(BUTTON_MODE_12, false);
    OutputPin mode_led(MODE_PIN_26);

    mode_button.init();
    mode_led.init();

    while (true)
    {

        if (mode_button.wait_for_long_press(5000))
        {
            ESP_LOGW("Inputs:", "Five seconds passed!");
            mode_led.toggle();

            if (mode_led.get_level() == 1)
            {
                server->set_dev_mode(DEV);
            }
            else
            {
                server->set_dev_mode(USER);
            }

            server->reset_handlers();
            vTaskDelay(pdMS_TO_TICKS(100));
            ESP_LOGI("Button", "Botón liberado");
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second Polling rate
    }
}

extern "C" void app_main(void)
{
    WifiHandler wifi;
    wifi.init();

    Server *server = new Server();
    server->start();

    // Main Loop: Prioridad alta (5) en Core 1 para respuesta instantánea
    xTaskCreatePinnedToCore(loop, "ButtonTask", 2048, (void *)server, 5, NULL, 1);

    // Core 1
    xTaskCreatePinnedToCore([](void *pvParameters)
                            {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(5000));
        } }, "MainTask", 4096, NULL, 5, NULL, 1);
}
