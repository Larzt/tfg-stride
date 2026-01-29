#ifndef HANDLER_WIFI_CONFIG_H
#define HANDLER_WIFI_CONFIG_H

#include "esp_http_server.h"

#include "wifi/wifi.h"
#include "wifi/credentials.h"

#include "esp_log.h"

extern httpd_uri_t uri_wifi_form_config;
extern httpd_uri_t uri_wifi_set_config;

#endif
