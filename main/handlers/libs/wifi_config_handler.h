#pragma once

#include "esp_http_server.h"
#include "handler.h"

class WifiConfigHandler : public Handler
{
public:
    WifiConfigHandler();

    httpd_uri_t *get_get_uri() override;
    httpd_uri_t *get_post_uri();

private:
    httpd_uri_t _get_uri;
    httpd_uri_t _post_uri;

    static esp_err_t get_handler(httpd_req_t *req);
    static esp_err_t post_handler(httpd_req_t *req);
};
