#include "WifiHandler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Server & Handlers (endpoints)
#include "server.h"
#include "status_endpoint.h"
#include "config_endpoint.h"

extern "C" void app_main(void)
{
    WifiHandler wifi;

    wifi.init();

    Server *server = new Server();
    server->add_handler(new StatusHandler());
    server->add_handler(new ConfigHandler());
    server->start();

    // Core 1
    xTaskCreatePinnedToCore([](void *pvParameters)
                            {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(5000));
        } }, "MainTask", 4096, NULL, 5, NULL, 1);
}
