#pragma once

#include <string>
#include "esp_http_server.h"
#include "esp_err.h"
#include "handler.h"

#define FILE_PATH "/sdcard/program.str"

/**
 * @brief Handler para un editor de texto en la SD card a través de HTTP.
 * Hereda de Handler para integrarse con el sistema de endpoints.
 */
class SdEditorHandler : public Handler
{
public:
  SdEditorHandler();

  // Implementación de Handler
  httpd_uri_t *get_get_uri() override; // Devuelve GET o POST según contexto (GET por defecto)

  // Si quieres exponer POST como otro endpoint, puedes agregar un getter específico
  httpd_uri_t *get_post_uri();

private:
  httpd_uri_t _get_uri;  // Endpoint GET
  httpd_uri_t _post_uri; // Endpoint POST

  // Handlers estáticos requeridos por ESP-IDF
  static esp_err_t get_handler(httpd_req_t *req);
  static esp_err_t post_handler(httpd_req_t *req);
  static std::string url_decode(const std::string &src);
};
