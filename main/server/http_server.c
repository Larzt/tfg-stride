#include "http_server.h"
#include "esp_log.h"

#include "wifi/wifi.h"
#include "config/constants.h"
#include "handlers/config/handler_wifi_config.h"
#include "handlers/internal/handler_root.h"
#include "handlers/internal/handler_status.h"
#include "handlers/internal/handler_config.h"
#include "handlers/internal/handler_led.h"
#include "handlers/external/handler_i2c_led.h"

httpd_handle_t start_webserver(void)
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_handle_t server = NULL;

  if (httpd_start(&server, &config) == ESP_OK)
  {
    if (is_wifi_in_ap_mode())
    {
      ESP_LOGW(LSERVER, "Servidor iniciado en modo AP; solo endpoints de configuración");
      httpd_register_uri_handler(server, &uri_root);
      httpd_register_uri_handler(server, &uri_config);
      httpd_register_uri_handler(server, &uri_wifi_form_config);
      httpd_register_uri_handler(server, &uri_wifi_set_config);
    }
    else
    {
      ESP_LOGI(LSERVER, "Servidor iniciado en modo STA; endpoints de operación normal");

      httpd_register_uri_handler(server, &uri_root);
      httpd_register_uri_handler(server, &uri_config);
      httpd_register_uri_handler(server, &uri_status);
      httpd_register_uri_handler(server, &uri_led);
      httpd_register_uri_handler(server, &uri_i2c_led);
    }
  }

  ESP_LOGI(LSERVER, "El servidor se ha inicializado correctamente");
  return server;
}
