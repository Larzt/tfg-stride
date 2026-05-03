file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cc" "*.cpp" "*.c")

message(STATUS "SOURCES: ${SOURCES}")

idf_component_register(
  SRCS ${SOURCES}

  INCLUDE_DIRS
  "."
  "types"
  "tasks"
  "tasks/include"
  "handler"
  "handler/include"
  "brain"
  "brain/machine"
  "brain/network"
  "brain/server"

  REQUIRES logger io observer
  REQUIRES esp_wifi esp_netif esp_event nvs_flash esp_http_server

  PRIV_REQUIRES esp_driver_gpio driver
)
