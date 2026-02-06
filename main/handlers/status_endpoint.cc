#include <string>
#include "utility.h"
#include "status_endpoint.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_clk.h"

StatusHandler::StatusHandler()
{
  _uri.uri = "/";
  _uri.method = HTTP_GET;
  _uri.handler = &StatusHandler::handler;
  _uri.user_ctx = nullptr;
}

httpd_uri_t *StatusHandler::get_uri()
{
  return &_uri;
}

esp_err_t StatusHandler::handler(httpd_req_t *req)
{
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  uint32_t free_heap = esp_get_free_heap_size();
  uint32_t cpu_freq = esp_clk_cpu_freq() / 1000000;

  std::string resp =
      "{" + kENDL +
      "  \"status\": \"ok\"," + kENDL +
      "  \"cores\": " + std::to_string(chip_info.cores) + "," + kENDL +
      "  \"chip_model\": " + std::to_string(chip_info.model) + "," + kENDL +
      "  \"revision\": " + std::to_string(chip_info.revision) + "," + kENDL +
      "  \"features\": " + std::to_string(chip_info.features) + "," + kENDL +
      "  \"cpu_freq_mhz\": " + std::to_string(cpu_freq) + "," + kENDL +
      "  \"free_heap\": " + std::to_string(free_heap) + kENDL +
      "}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), resp.length());

  return ESP_OK;
}
