idf_component_register(
    SRCS
        "main.c"
        "constants.c"

        # Carpetas del proyecto
        "wifi/wifi.c"
        "wifi/credentials.c"
        "server/http_server.c"

        # Handlers HTTP
        "server/handlers/handler_root.c"
        "server/handlers/handler_config.c"
        "server/handlers/handler_status.c"
        "server/handlers/handler_led.c"

        # Drivers
        "drivers/led_driver.c"

    PRIV_INCLUDE_DIRS "${IDF_PATH}/components/esp_hw_support/include/esp_private"

    INCLUDE_DIRS
        "."                 # main/
        "./wifi"
        "./server"
        "./server/handlers"
        "./drivers"
        "./config"

    REQUIRES esp_http_server esp_wifi driver nvs_flash json

    EMBED_FILES "config.json"
)
