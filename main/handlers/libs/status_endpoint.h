#pragma once

#include "handler.h"

class StatusHandler : public Handler
{
public:
  StatusHandler();

  httpd_uri_t *get_get_uri() override;

private:
  static esp_err_t handler(httpd_req_t *req);
  httpd_uri_t _uri;
};
