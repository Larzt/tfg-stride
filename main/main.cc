#include "WifiHandler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Server & Handlers (endpoints)
#include "server.h"
#include "root_endpoint.h"
#include "status_endpoint.h"
#include "config_endpoint.h"

// button headers & variables
#include "driver/gpio.h"
#define BOOT_BUTTON_PIN GPIO_NUM_23
#define TIME_PRESSED 5000

void load_handlers(Server *server)
{
    if (server->is_dev_mode())
    {
        server->add_handler(new StatusHandler());
        server->add_handler(new ConfigHandler());
    }
    else
    {
        server->add_handler(new RootHandler());
    }
}

void loop(void *pvParameters)
{
    // Configuración inicial del pin (GPIO 23 con Pull-up)
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BOOT_BUTTON_PIN);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    bool system_active = false;
    TickType_t start_press_time = 0;
    bool is_pressing = false;
    bool action_executed = false;

    while (true)
    {
        int current_state = gpio_get_level(BOOT_BUTTON_PIN);
        TickType_t now = xTaskGetTickCount();

        // 1. Detectar inicio de pulsación (Ahora es 1 cuando presionas)
        if (current_state == 1 && !is_pressing)
        {
            is_pressing = true;
            start_press_time = now;
            action_executed = false;
            ESP_LOGI("Button", "Botón presionado, manteniendo...");
        }

        // 2. Si se mantiene presionado (1), calcular el tiempo
        if (is_pressing && !action_executed)
        {
            uint32_t duration_ms = (now - start_press_time) * portTICK_PERIOD_MS;

            if (duration_ms >= TIME_PRESSED)
            {
                system_active = !system_active;
                action_executed = true;
                ESP_LOGW("Button", "¡%d SEGUNDOS ALCANZADOS! Variable cambiada a: %s",
                         TIME_PRESSED / 1000, system_active ? "TRUE" : "FALSE");
            }
        }

        // 3. Detectar cuando se suelta el botón (Ahora es 0 al soltar)
        if (current_state == 0 && is_pressing)
        {
            is_pressing = false;
            uint32_t final_duration = (now - start_press_time) * portTICK_PERIOD_MS;

            if (!action_executed)
            {
                ESP_LOGI("Button", "Se soltó demasiado pronto (solo duró %lu s)", final_duration / 1000);
            }
            else
            {
                ESP_LOGI("Button", "Botón liberado tras ejecución.");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

extern "C" void app_main(void)
{
    WifiHandler wifi;

    wifi.init();

    Server *server = new Server();
    load_handlers(server);
    server->start();

    // Tarea del Botón: Prioridad alta (5) en Core 1 para respuesta instantánea
    xTaskCreatePinnedToCore(loop, "ButtonTask", 2048, NULL, 5, NULL, 1);

    // Core 1
    xTaskCreatePinnedToCore([](void *pvParameters)
                            {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(5000));
        } }, "MainTask", 4096, NULL, 5, NULL, 1);
}
