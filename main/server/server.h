#pragma once
#include "network.h"
#include <vector>
#include "esp_http_server.h"
#include "handler.h"

enum ServerMode
{
  DEV,
  USER,
};

class Server
{
public:
  Server();
  virtual ~Server();

  void add_handler(Handler *handler);
  httpd_handle_t start();
  void reset_handlers();
  inline bool is_dev_mode() { return _mode == DEV ? true : false; }
  inline void set_dev_mode(ServerMode mode) { _mode = mode; }
  inline bool is_ap_mode() { return Network::current_mode() == WifiMode::AP; }

protected:
  virtual uint16_t port() const { return 80; }

private:
  httpd_handle_t _server;
  ServerMode _mode;
  std::vector<Handler *> _handlers;

  void load_handlers();
  void register_handlers();
};
