#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"

#include "wifi/wifi.h"
#include "server/server.h"
#include "config/logs.h"

void app_main(void)
{
    // Inicializa NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Inicializa WiFi como AP
    wifi_init_softap_sta();

    // Inicia servidor HTTP
    start_webserver();

    ESP_LOGI(kLogMode[MAIN], "Sistema inicializado correctamente.");
}
