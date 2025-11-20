#pragma once

#include "esp_http_server.h"

// Inicializa el servidor HTTP y registra todos los handlers
httpd_handle_t start_webserver(void);
