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
  httpd_uri_t *get_post_uri() override;

private:
  httpd_uri_t _get_uri;
  httpd_uri_t _create_uri;
  static esp_err_t get_handler(httpd_req_t *req);
  static esp_err_t create_handler(httpd_req_t *req);


  // Estructura auxiliar para representar archivos
  struct FileInfo
  {
    std::string name;
    size_t size;
    bool is_dir;
  };
};
