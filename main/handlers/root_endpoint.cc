#include <string>
#include "utility.h"
#include "root_endpoint.h"

#include "esp_log.h"

RootHandler::RootHandler()
{
  _uri.uri = "/";
  _uri.method = HTTP_GET;
  _uri.handler = &RootHandler::handler;
  _uri.user_ctx = nullptr;
}

httpd_uri_t *RootHandler::get_get_uri()
{
  return &_uri;
}

esp_err_t RootHandler::handler(httpd_req_t *req)
{
  std::string resp = "{" + kENDL;
  resp += "  \"message\": ok";
  resp += "}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), resp.length());

  return ESP_OK;
}
