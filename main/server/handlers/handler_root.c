#include "handler_root.h"

esp_err_t root_get_handler(httpd_req_t *req)
{
  const char resp[] = "<!DOCTYPE html>"
                      "<html>"
                      "<body>"
                      "<h1>Hola desde ESP32 AP!</h1>"
                      "</body>"
                      "</html>";

  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

httpd_uri_t uri_root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL};
