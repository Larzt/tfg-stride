#pragma once

#include "handler.h"

class RootHandler : public EndpointHandler
{
public:
  RootHandler();

  httpd_uri_t *get_uri() override;

private:
  static esp_err_t handle(httpd_req_t *req);
  httpd_uri_t uri_;
};
