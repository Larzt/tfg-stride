#include "root_endpoint.h"

RootHandler::RootHandler()
{
  uri_.uri = "/";
  uri_.method = HTTP_GET;
  uri_.handler = &RootHandler::handle;
  uri_.user_ctx = nullptr;
}

httpd_uri_t *RootHandler::get_uri()
{
  return &uri_;
}

esp_err_t RootHandler::handle(httpd_req_t *req)
{
  const char *resp = "Hola desde RootHandler";
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}
