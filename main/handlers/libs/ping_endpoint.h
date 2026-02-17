#pragma once

#include "handler.h"
#include "output.h"

class PingHandler : public Handler
{
public:
  PingHandler();

  httpd_uri_t *get_uri() override;

private:
  static OutputPin user_ping_led;
  static esp_err_t handler(httpd_req_t *req);
  httpd_uri_t _uri;
};
