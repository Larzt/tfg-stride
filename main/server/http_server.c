#include "http_server.h"
#include "esp_log.h"

#include "config/constants.h"
#include "handlers/handler_root.h"
#include "handlers/handler_status.h"
#include "handlers/handler_led.h"

httpd_handle_t start_webserver(void)
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_handle_t server = NULL;

  if (httpd_start(&server, &config) == ESP_OK)
  {
    httpd_register_uri_handler(server, &uri_root);
    httpd_register_uri_handler(server, &uri_status);
    httpd_register_uri_handler(server, &uri_led);
  }

  ESP_LOGI(LSERVER, "El servidor se ha inicializado correctamente");
  return server;
}
