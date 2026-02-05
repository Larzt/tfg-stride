#include "server.h"

#include "esp_log.h"
#include "wifi/wifi.h"

static const char *TAG = "SERVER";

Server::Server() : server_(nullptr) {}

Server::~Server()
{
  if (server_)
  {
    httpd_stop(server_);
  }
}

void Server::add_handler(Handler *handler)
{
  handlers_.push_back(handler);
}

httpd_handle_t Server::start()
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = port();

  if (httpd_start(&server_, &config) != ESP_OK)
  {
    ESP_LOGE(TAG, "No se pudo iniciar el servidor");
    return nullptr;
  }

  register_handlers();

  ESP_LOGI(TAG, "Servidor HTTP iniciado");
  return server_;
}

void Server::register_handlers()
{
  bool ap_mode = is_wifi_in_ap_mode();

  for (auto *handler : handlers_)
  {
    if ((ap_mode && handler->enabled_in_ap_mode()) ||
        (!ap_mode && handler->enabled_in_sta_mode()))
    {

      httpd_register_uri_handler(server_, handler->get_uri());
    }
  }
}
