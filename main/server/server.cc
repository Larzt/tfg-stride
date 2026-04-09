#include "server.h"

#include "esp_log.h"

#include "root_endpoint.h"
#include "ping_endpoint.h"
#include "status_endpoint.h"
#include "config_endpoint.h"
#include "sd_editor_endpoint.h"
#include "sd_reader_endpoint.h"
#include "sd_browser_endpoint.h"
#include "wifi_config_handler.h"

static const char *TAG = "SERVER";

Server::Server() : _server(nullptr), _mode(DEV) {}

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
  config.max_req_hdr_len = 1024;   // Amplia el espacio del header
  config.max_uri_handlers = 12;    // Asegura espacio para los endpoints
  config.lru_purge_enable = true;  // Cierra conexiones viejas si se satura

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
                                 handler->get_get_uri()->uri,
                                 handler->get_get_uri()->method);
    if (handler->get_post_uri() != nullptr)
    {
      httpd_unregister_uri_handler(_server,
                                   handler->get_post_uri()->uri,
                                   handler->get_post_uri()->method);
    }
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
  this->add_handler(new RootHandler());
  this->add_handler(new WifiConfigHandler());

  this->add_handler(new SdEditorHandler());
  this->add_handler(new SdBrowserHandler());
  this->add_handler(new SdReaderHandler());

  if (this->is_dev_mode())
  {
    this->add_handler(new StatusHandler());
    this->add_handler(new ConfigHandler());
    this->add_handler(new PingHandler());
  }
}

void Server::register_handlers()
{
  for (Handler *handler : _handlers)
  {
    if (httpd_register_uri_handler(_server, handler->get_get_uri()) != ESP_OK)
    {
      ESP_LOGE("SERVER", "Error registering GET");
    }

    if (handler->get_post_uri() != nullptr)
    {
      if (httpd_register_uri_handler(_server, handler->get_post_uri()) != ESP_OK)
      {
        ESP_LOGE("SERVER", "Error registering POST");
      }
    }
  }
}
