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

// Handle de la tarea principal (servidor)
static TaskHandle_t mainTaskHandle = NULL;

/* ============================================================
   TAREA PRINCIPAL (lógica pesada - servidor)
   ============================================================ */
void main_task(void *pvParameters)
{
    Server *server = (Server *)pvParameters;
    uint32_t led_state;

    ESP_LOGI("SYSTEM", "MainTask iniciada");

    while (true)
    {
        // Espera notificación del botón
        if (xTaskNotifyWait(0, 0, &led_state, portMAX_DELAY))
        {
            ESP_LOGI("SYSTEM", "Cambio de modo recibido");

            if (led_state == 1)
            {
                ESP_LOGI("SYSTEM", "Modo DEV activado");
                server->set_dev_mode(DEV);
            }
            else
            {
                ESP_LOGI("SYSTEM", "Modo USER activado");
                server->set_dev_mode(USER);
            }

            // Operación pesada
            server->reset_handlers();

            ESP_LOGI("SYSTEM", "Servidor reconfigurado");
        }
    }
}

/* ============================================================
   TAREA DEL BOTÓN (ligera)
   ============================================================ */
void button_task(void *pvParameters)
{
    InputPin mode_button(BUTTON_MODE_12, false);
    OutputPin mode_led(MODE_PIN_26);

    mode_button.init();
    mode_led.init();

    ESP_LOGI("SYSTEM", "ButtonTask iniciada");

    while (true)
    {
        if (mode_button.wait_for_long_press(TIME_PRESSED))
        {
            ESP_LOGW("Inputs", "Five seconds passed!");

            mode_led.toggle();

            if (mainTaskHandle != NULL)
            {
                xTaskNotify(
                    mainTaskHandle,
                    mode_led.get_level(),
                    eSetValueWithOverwrite);
            }

            ESP_LOGI("Button", "Botón liberado");
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // polling rápido y seguro
    }
}

/* ============================================================
   APP MAIN
   ============================================================ */
extern "C" void app_main(void)
{
    WifiHandler wifi;
    wifi.init();

    Server *server = new Server();
    server->start();

    // Tarea principal (servidor - más stack)
    xTaskCreatePinnedToCore(
        main_task,
        "MainTask",
        6144, // stack suficiente para HTTP + C++
        (void *)server,
        5,
        &mainTaskHandle,
        1);

    // Tarea del botón (ligera)
    xTaskCreatePinnedToCore(
        button_task,
        "ButtonTask",
        4096,
        NULL,
        5,
        NULL,
        1);
}
