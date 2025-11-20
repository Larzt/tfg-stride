#pragma once

#include "driver/gpio.h"

#include "esp_http_server.h"

#include "esp_log.h"
#include "esp_clk.h"
#include "esp_chip_info.h"

esp_err_t root_get_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);
