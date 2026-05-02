#pragma once
#include <vector>
#include "esp_http_server.h"

#include "network.h"
#include "handler.h"

enum ServerMode
{
  Deverloper,
  User,
};

class Server
{

public:
  Server();
  ~Server() {};

  void start_server();

  void add_handler(Handler *handler);
  void reset_handlers();

  inline ServerMode get_server_mode() { return _mode; }
  inline void set_server_mode(ServerMode mode) { _mode = mode; }


private:
  httpd_handle_t _server;
  std::vector<Handler *> _handlers;

  void load_handlers();
  void register_handlers();
};
