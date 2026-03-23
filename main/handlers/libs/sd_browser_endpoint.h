#pragma once

#include <string>
#include <vector>
#include "esp_http_server.h"
#include "handler.h"

class SdBrowserHandler : public Handler
{
public:
  SdBrowserHandler();
  httpd_uri_t *get_get_uri() override;

private:
  httpd_uri_t _get_uri;
  static esp_err_t get_handler(httpd_req_t *req);

  // Estructura auxiliar para representar archivos
  struct FileInfo
  {
    std::string name;
    size_t size;
    bool is_dir;
  };
};
