#ifndef ENDPOINT_HANDLER_H
#define ENDPOINT_HANDLER_H

#include "esp_http_server.h"

class Handler
{
public:
  virtual ~Handler() = default;

  // Devuelve la definición del endpoint
  virtual httpd_uri_t *get_uri() = 0;

  // Permite habilitar/deshabilitar según modo (AP / STA)
  virtual bool enabled_in_ap_mode() const { return true; }
  virtual bool enabled_in_sta_mode() const { return true; }
};

#endif
