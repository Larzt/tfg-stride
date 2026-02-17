#include "server.h"

#include "esp_log.h"
#include "WifiHandler.h"

#include "root_endpoint.h"
#include "ping_endpoint.h"
#include "status_endpoint.h"
#include "config_endpoint.h"

static const char *TAG = "SERVER";

Server::Server() : _server(nullptr), _mode(USER) {}

Server::~Server()
{
  if (_server)
  {
    httpd_stop(_server);
  }
}

void Server::add_handler(Handler *handler)
{
  _handlers.push_back(handler);
}

httpd_handle_t Server::start()
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = port();

  if (httpd_start(&_server, &config) != ESP_OK)
  {
    ESP_LOGE(TAG, "No se pudo iniciar el servidor");
    return nullptr;
  }

  load_handlers();
  register_handlers();

  ESP_LOGI("SERVER", "Servidor escuchando en el puerto: %d", config.server_port);
  return _server;
}

void Server::reset_handlers()
{
  ESP_LOGW(TAG, "Limpiando rutas del servidor HTTP...");

  for (auto *handler : _handlers)
  {
    httpd_unregister_uri_handler(_server,
                                 handler->get_uri()->uri,
                                 handler->get_uri()->method);
  }

  for (auto *handler : _handlers)
  {
    delete handler;
  }
  _handlers.clear();

  load_handlers();
  register_handlers();
}

void Server::load_handlers()
{
  if (this->is_dev_mode())
  {
    ESP_LOGW("Server mode: ", "Dev mode");
    this->add_handler(new StatusHandler());
    this->add_handler(new ConfigHandler());
  }
  else
  {
    ESP_LOGW("Server mode: ", "User mode");
    this->add_handler(new RootHandler());
    this->add_handler(new PingHandler());
  }
}

void Server::register_handlers()
{
  // bool ap_mode = is_wifi_in_ap_mode();

  for (auto *handler : _handlers)
  {
    // if ((ap_mode && handler->enabled_in_ap_mode()) ||
    // (!ap_mode && handler->enabled_in_sta_mode()))
    // { }
    httpd_register_uri_handler(_server, handler->get_uri());
  }
}
