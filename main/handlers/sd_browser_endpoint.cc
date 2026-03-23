#include "sd_browser_endpoint.h"
#include <dirent.h>
#include <sys/stat.h>
#include "esp_log.h"
#include <sstream>

#define MOUNT_POINT "/sdcard"

SdBrowserHandler::SdBrowserHandler()
{
  _get_uri.uri = "/files";
  _get_uri.method = HTTP_GET;
  _get_uri.handler = &SdBrowserHandler::get_handler;
  _get_uri.user_ctx = nullptr;
}

httpd_uri_t *SdBrowserHandler::get_get_uri()
{
  return &_get_uri;
}

esp_err_t SdBrowserHandler::get_handler(httpd_req_t *req)
{
  DIR *dir = opendir(MOUNT_POINT);
  if (!dir)
  {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No se pudo leer la SD");
    return ESP_FAIL;
  }

  std::stringstream html;
  html << "<html><head><title>STRIDE Explorer</title><meta charset='UTF-8'></head><body>";
  html << "<h2>Explorador de Archivos (SD)</h2>";
  html << "<table border='1'><tr><th>Nombre</th><th>Acciones</th></tr>";

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {
    std::string fileName = entry->d_name;

    // Saltamos archivos ocultos o sistema
    if (fileName == "." || fileName == "..")
      continue;

    html << "<tr>";
    html << "<td>" << fileName << "</td>";
    html << "<td>";
    // Enlace para Editar (usando tu endpoint existente)
    // Pasamos el nombre del archivo como parámetro query: ?path=nombre
    html << "<a href='/editor?file=" << fileName << "'>[Editar]</a> ";
    // Enlace para Ver (puedes crear otro handler similar o descargar el archivo)
    html << "<a href='/view?file=" << fileName << "'>[Ver]</a>";
    html << "</td></tr>";
  }
  closedir(dir);

  html << "</table></body></html>";

  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, html.str().c_str(), html.str().length());
  return ESP_OK;
}
