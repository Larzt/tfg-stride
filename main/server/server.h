#pragma once

#include <vector>
#include "esp_http_server.h"
#include "handler.h"

class Server
{
public:
  Server();
  virtual ~Server();

  void add_handler(Handler *handler);
  httpd_handle_t start();

protected:
  virtual uint16_t port() const { return 80; }

private:
  httpd_handle_t _server;
  std::vector<Handler *> _handlers;

  void register_handlers();
};
