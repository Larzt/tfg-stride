#include "sd_reader_endpoint.h"
#include <stdio.h>
#include <sstream>
#include "esp_log.h"

#define SD_BASE_PATH "/sdcard"

SdReaderHandler::SdReaderHandler()
{
  _get_uri.uri = "/view";
  _get_uri.method = HTTP_GET;
  _get_uri.handler = &SdReaderHandler::get_handler;
  _get_uri.user_ctx = nullptr;
}

httpd_uri_t *SdReaderHandler::get_get_uri()
{
  return &_get_uri;
}

esp_err_t SdReaderHandler::get_handler(httpd_req_t *req)
{
  char query[128];
  char filepath[128];
  bool file_found = false;

  if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK)
  {
    char filename[64];
    if (httpd_query_key_value(query, "file", filename, sizeof(filename)) == ESP_OK)
    {
      snprintf(filepath, sizeof(filepath), "%s/%s", SD_BASE_PATH, filename);
      file_found = true;
    }
  }

  if (!file_found)
  {
    ESP_LOGW("SD_READER", "Petición sin parámetro 'file'");
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Falta el parámetro 'file'");
    return ESP_FAIL;
  }

  FILE *f = fopen(filepath, "r");
  if (!f)
  {
    ESP_LOGE("SD_READER", "Error al abrir: %s", filepath);
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Archivo no encontrado en SD");
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "text/plain; charset=utf-8");

  char line[256];
  while (fgets(line, sizeof(line), f))
  {
    httpd_resp_sendstr_chunk(req, line);
  }

  httpd_resp_sendstr_chunk(req, NULL);
  fclose(f);

  ESP_LOGI("SD_READER", "Archivo servido: %s", filepath);
  return ESP_OK;
}
