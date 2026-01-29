#include "handler_wifi_config.h"
#include "config/routes.h"
#include "wifi/credentials.h"

esp_err_t get_esp_wifi_form_config(httpd_req_t *req)
{
  // ROUTE: ESP_WIFI_SET_CONFIG = /set_wifi
  const char *resp = "<!DOCTYPE html>"
                     "<html><head><meta charset='utf-8'/>"
                     "<title>Configurar WiFi</title></head>"
                     "<body>"
                     "<h2>Configuración WiFi</h2>"
                     "<form action=\"/set_wifi\" method=\"post\">"
                     "SSID:<br><input name=\"ssid\"><br><br>"
                     "Contraseña:<br><input name=\"pass\" type=\"password\"><br><br>"
                     "<input type=\"submit\" value=\"Guardar\">"
                     "</form></body></html>";

  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

  return ESP_OK;
}

esp_err_t wifi_config_post_handler(httpd_req_t *req)
{
  char buf[128];
  int len = httpd_req_recv(req, buf, sizeof(buf) - 1);

  if (len <= 0)
    return ESP_FAIL;

  buf[len] = 0;

  // Variables donde guardaremos en RAM lo recibido
  char ssid[32] = {0};
  char pass[64] = {0};

  // Parsear formato tipo: ssid=mywifi&pass=12345678
  sscanf(buf, "ssid=%31[^&]&pass=%63s", ssid, pass);

  ESP_LOGI("WIFI", "Nuevas credenciales: SSID=%s PASS=%s", ssid, pass);

  // Guardar en NVS
  save_wifi_credentials(ssid, pass);

  // Enviar respuesta
  httpd_resp_sendstr(req, "Credenciales guardadas. Reiniciando STA...");

  // Cambiar a STA
  start_sta_mode();

  return ESP_OK;
}

httpd_uri_t uri_wifi_form_config = {
    .uri = ESP_WIFI_FORM_CONFIG,
    .method = HTTP_GET,
    .handler = get_esp_wifi_form_config,
    .user_ctx = NULL};

httpd_uri_t uri_wifi_set_config = {
    .uri = ESP_WIFI_SET_CONFIG,
    .method = HTTP_POST,
    .handler = wifi_config_post_handler,
    .user_ctx = NULL};
