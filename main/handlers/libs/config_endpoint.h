#pragma once

#include "handler.h"

class ConfigHandler : public Handler
{
public:
  ConfigHandler();

  httpd_uri_t *get_uri() override;

private:
  static esp_err_t handler(httpd_req_t *req);
  httpd_uri_t _uri;
};
