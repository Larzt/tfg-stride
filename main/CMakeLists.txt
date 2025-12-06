idf_component_register(
    SRCS
        "main.c"
        "constants.c"

        # Carpetas del proyecto
        "wifi/wifi.c"
        "wifi/credentials.c"
        "server/http_server.c"

        # Handlers HTTP
        # Internal
        "server/handlers/internal/handler_root.c"
        "server/handlers/internal/handler_config.c"
        "server/handlers/internal/handler_status.c"
        "server/handlers/internal/handler_led.c"
        # External
        "server/handlers/external/handler_i2c_led.c"

        # Drivers
        "drivers/led_driver.c"
        "drivers/pcf_driver.c"

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
