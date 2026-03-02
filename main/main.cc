#include "WifiHandler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "interpreter.h"

// Server & Handlers
#include "server.h"

// Controllers
#include "input.h"
#include "output.h"
#include "main.h"

#define TIME_PRESSED 5000

#define MODE_PIN_26 GPIO_NUM_26
#define BUTTON_MODE_12 GPIO_NUM_12

// Handle solo de la tarea principal
static TaskHandle_t mainTaskHandle = NULL;

/* ============================================================
   TAREA PRINCIPAL (servidor - pesada)
   ============================================================ */
void main_task(void *pvParameters)
{
    Server *server = (Server *)pvParameters;
    uint32_t led_state;

    ESP_LOGI("SYSTEM", "MainTask iniciada");

    while (true)
    {
        if (xTaskNotifyWait(0, 0, &led_state, portMAX_DELAY))
        {
            ESP_LOGI("SYSTEM", "Cambio de modo recibido");

            if (led_state == 1)
            {
                server->set_dev_mode(DEV);
                ESP_LOGI("SYSTEM", "Modo DEV activado");
            }
            else
            {
                server->set_dev_mode(USER);
                ESP_LOGI("SYSTEM", "Modo USER activado");
            }

            server->reset_handlers();
            ESP_LOGI("SYSTEM", "Servidor reconfigurado");
        }
    }
}

/* ============================================================
   TAREA DEL BOTÓN (solo controla servidor)
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

            ESP_LOGI("Button", "Modo cambiado");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* ============================================================
   TAREA TEST INTERPRETER (INDEPENDIENTE)
   ============================================================ */
void test_str_programs(void *params)
{
    Interpreter interpreter;

    // Crear LED virtual del lenguaje
    auto tokens = Tokenize(R"(
output=led name=myLed pin=16
)");
    interpreter.execute(tokens);

    ESP_LOGI("TEST", "TestStrPrograms iniciada");

    while (true)
    {
        tokens = Tokenize("write=myLed on");
        interpreter.execute(tokens);
        ESP_LOGI("TEST", "LED ON");

        vTaskDelay(pdMS_TO_TICKS(2000));

        tokens = Tokenize("write=myLed off");
        interpreter.execute(tokens);
        ESP_LOGI("TEST", "LED OFF");

        vTaskDelay(pdMS_TO_TICKS(2000));
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

    // Tarea servidor
    xTaskCreatePinnedToCore(
        main_task,
        "MainTask",
        6144,
        (void *)server,
        5,
        &mainTaskHandle,
        1);

    // Tarea botón
    xTaskCreatePinnedToCore(
        button_task,
        "ButtonTask",
        4096,
        NULL,
        5,
        NULL,
        1);

    // Tarea independiente de test
    xTaskCreatePinnedToCore(
        test_str_programs,
        "TestTask",
        4096,
        NULL,
        4, // prioridad ligeramente menor
        NULL,
        1);
}
