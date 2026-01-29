#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "wifi/wifi.h"
#include "server/http_server.h"
#include "config/constants.h"

#include "pcf_driver.h"

void app_main(void)
{
    // Inicializa NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Inicializa WiFi en modo AP + STA
    init_wifi();

    // // Inicializa PCF8574 (driver I2C)
    // if (pcf8574_init(0x27) != ESP_OK)
    // {
    //     ESP_LOGE(LMAIN, "Error inicializando PCF8574");
    //     return; // Si falla, no seguimos
    // }

    // Inicia servidor HTTP
    start_webserver();

    ESP_LOGI(LMAIN, "Sistema inicializado correctamente.");
}
