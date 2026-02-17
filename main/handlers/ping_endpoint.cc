#include <string>
#include "utility.h"
#include "ping_endpoint.h"

#define USER_PIN_25 GPIO_NUM_25

OutputPin PingHandler::user_ping_led(USER_PIN_25);

PingHandler::PingHandler()
{
  user_ping_led.init();
  _uri.uri = "/ping";
  _uri.method = HTTP_GET;
  _uri.handler = &PingHandler::handler;
  _uri.user_ctx = nullptr;
}

httpd_uri_t *PingHandler::get_uri()
{
  return &_uri;
}

esp_err_t PingHandler::handler(httpd_req_t *req)
{
  user_ping_led.toggle();

  std::string resp = "{" + kENDL;
  resp += "  \"status\": \"pong\"," + kENDL;
  resp += "  \"led_state\": " + std::to_string(user_ping_led.get_level()) + kENDL;
  resp += "}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), resp.length());
  return ESP_OK;
}
