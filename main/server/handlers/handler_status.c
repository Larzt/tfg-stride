#include "handler_status.h"
#include "config/routes.h"

#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_clk.h"

esp_err_t get_esp_status(httpd_req_t *req)
{
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  uint32_t free_heap = esp_get_free_heap_size();
  uint32_t cpu_freq = esp_clk_cpu_freq() / 1000000;

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

httpd_uri_t uri_status = {
    .uri = GET_ESP32_STATUS,
    .method = HTTP_GET,
    .handler = get_esp_status,
    .user_ctx = NULL};
