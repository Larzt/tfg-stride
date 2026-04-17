# archivos .cc, .cpp y .c en la carpeta actual y subcarpetas
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cc" "*.cpp" "*.c")

list(FILTER SOURCES EXCLUDE REGEX ".*/tests/.*")

message(STATUS "SOURCES: ${SOURCES}")

idf_component_register(
    SRCS ${SOURCES}


    PRIV_INCLUDE_DIRS "${IDF_PATH}/components/esp_hw_support/include/esp_private"

    INCLUDE_DIRS
        "."
        "./log"
        "./language"
        "./language/lexer"
        "./network"
        "./server"
        "./handlers"
        "./handlers/libs"
        "./controllers"
        "./controllers/libs"
        "./consts"
        "./utils"

    REQUIRES esp_http_server esp_wifi driver nvs_flash

    PRIV_REQUIRES fatfs sdmmc spi_flash
)
