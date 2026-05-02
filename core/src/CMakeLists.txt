file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cc" "*.cpp" "*.c")

idf_component_register(
  SRCS "main.cc"
  INCLUDE_DIRS "."
  REQUIRES logger
  PRIV_REQUIRES esp_driver_gpio driver
)
