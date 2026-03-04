#pragma once
#include "esp_http_server.h"
#include "Handler.h"
#include <string>

class SdReaderHandler : public Handler
{
public:
  SdReaderHandler(const std::string &path);

  httpd_uri_t *get_get_uri() override; // GET handler
  virtual bool enabled_in_ap_mode() const override { return true; }
  virtual bool enabled_in_sta_mode() const override { return true; }

private:
  static esp_err_t get_handler(httpd_req_t *req);

  std::string _file_path;
  httpd_uri_t _get_uri;
};
