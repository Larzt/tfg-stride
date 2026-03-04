#include "sd_reader_endpoint.h"
#include <stdio.h>
#include <sstream>
#include "esp_log.h"

SdReaderHandler::SdReaderHandler(const std::string &path) : _file_path(path)
{
  _get_uri.uri = "/read_sd";
  _get_uri.method = HTTP_GET;
  _get_uri.handler = &SdReaderHandler::get_handler;
  _get_uri.user_ctx = this;
}

httpd_uri_t *SdReaderHandler::get_get_uri()
{
  return &_get_uri;
}

esp_err_t SdReaderHandler::get_handler(httpd_req_t *req)
{
  // Recuperar la instancia de la clase desde user_ctx
  SdReaderHandler *self = (SdReaderHandler *)req->user_ctx;
  if (!self)
    return ESP_FAIL;

  FILE *f = fopen(self->_file_path.c_str(), "r");
  if (!f)
  {
    ESP_LOGE("SD_READER", "No se pudo abrir archivo: %s", self->_file_path.c_str());
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Archivo no encontrado");
    return ESP_FAIL;
  }

  std::stringstream file_content;
  char line[256];
  while (fgets(line, sizeof(line), f))
  {
    file_content << line;
  }
  fclose(f);

  std::string response = file_content.str();
  httpd_resp_set_type(req, "text/plain");
  httpd_resp_send(req, response.c_str(), response.length());

  return ESP_OK;
}
