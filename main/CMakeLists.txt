idf_component_register(
    SRCS "wifi/wifi.c" "main.c" "server/server.c"
    INCLUDE_DIRS "."
    PRIV_INCLUDE_DIRS "${IDF_PATH}/components/esp_hw_support/include/esp_private"
    REQUIRES esp_wifi esp_netif esp_event nvs_flash esp_http_server driver
)
