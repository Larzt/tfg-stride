#include "handler_config.h"
#include "config/routes.h"
#include "wifi/credentials.h"

esp_err_t get_esp_config(httpd_req_t *req)
{
  wifi_credentials_t creds = get_wifi_credentials();

  char resp[256];
  snprintf(resp, sizeof(resp),
           "{"
           "\"ESP_AP\":{"
           "\"SSID\":\"%s\","
           "\"PASS\":\"%s\""
           "},"
           "\"LOCAL\":{"
           "\"SSID\":\"%s\","
           "\"PASS\":\"%s\""
           "}"
           "}",
           creds.ap_ssid,
           creds.ap_pass,
           creds.local_ssid,
           creds.local_pass);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

  return ESP_OK;
}

httpd_uri_t uri_config = {
    .uri = ESP_CONFIG,
    .method = HTTP_GET,
    .handler = get_esp_config,
    .user_ctx = NULL};
