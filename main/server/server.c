#include "server.h"
#include "config/routes.h"

/* ==================== HTTP Root ==================== */
esp_err_t root_get_handler(httpd_req_t *req)
{
  const char resp[] = "<!DOCTYPE html><html><body><h1>¡Hola desde ESP32 AP!</h1></body></html>";
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL};

/* ==================== HTTP ESP32 ==================== */
/* ==== Status ====*/
esp_err_t get_esp_status(httpd_req_t *req)
{
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  uint32_t free_heap = esp_get_free_heap_size();
  uint32_t cpu_freq = esp_clk_cpu_freq() / 1000000; // MHz

  char resp[256];
  snprintf(resp, sizeof(resp),
           "{"
           "\"status\":\"ok\","
           "\"cores\":%d,"
           "\"chip_model\":%d,"
           "\"revision\":%d,"
           "\"features\":%ld,"
           "\"cpu_freq_mhz\":%ld,"
           "\"free_heap\":%ld"
           "}",
           chip_info.cores,
           chip_info.model,
           chip_info.revision,
           chip_info.features,
           cpu_freq,
           free_heap);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

httpd_uri_t uri_esp_status = {
    .uri = GET_ESP32_STATUS,
    .method = HTTP_GET,
    .handler = get_esp_status,
    .user_ctx = NULL};

/* ==== PIN LED ====*/
const int LED_P = 26;
uint8_t boolState = 0;

esp_err_t put_pin_led(httpd_req_t *req)
{

  gpio_config_t io_conf = {
      .pin_bit_mask = (1ULL << LED_P),
      .mode = GPIO_MODE_INPUT_OUTPUT};

  gpio_config(&io_conf);

  boolState ^= (1 << 0);

  gpio_set_level(LED_P, boolState);

  char resp[256];
  snprintf(resp, sizeof(resp),
           "{"
           "\n Pin led: %d"
           "\n state: %d"
           "}",
           LED_P,
           boolState);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

httpd_uri_t uri_put_led = {
    .uri = PUT_LED,
    .method = HTTP_PUT,
    .handler = put_pin_led,
    .user_ctx = NULL};

/* ==================== HTTP Server Init ==================== */
httpd_handle_t
start_webserver(void)
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80; // puerto HTTP

  httpd_handle_t server = NULL;
  if (httpd_start(&server, &config) == ESP_OK)
  {
    httpd_register_uri_handler(server, &uri_get);
    httpd_register_uri_handler(server, &uri_esp_status);
    httpd_register_uri_handler(server, &uri_put_led);
  }
  return server;
}
