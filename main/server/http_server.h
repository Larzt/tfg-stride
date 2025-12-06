#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_http_server.h"

// Inicializa el servidor HTTP y registra todos los handlers
httpd_handle_t start_webserver(void);

#endif
