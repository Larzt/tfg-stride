idf_component_register(
    SRCS
        "main.cc"
        "wifi/WifiHandler.cc"
        "server/server.cc"

    # PRIV_INCLUDE_DIRS "${IDF_PATH}/components/esp_hw_support/include/esp_private"

    INCLUDE_DIRS
        "."
        "./wifi"
        "./server"
        "./handlers"

    REQUIRES esp_http_server esp_wifi driver nvs_flash json

    # EMBED_FILES "config.json"
)
