#include "sd_browser_endpoint.h"
#include <dirent.h>
#include <sys/stat.h>
#include "esp_log.h"
#include <sstream>

#define MOUNT_POINT "/sdcard"

SdBrowserHandler::SdBrowserHandler()
{
  _get_uri.uri = "/browser";
  _get_uri.method = HTTP_GET;
  _get_uri.handler = &SdBrowserHandler::get_handler;
  _get_uri.user_ctx = nullptr;

  _create_uri.uri = "/create";
  _create_uri.method = HTTP_GET;
  _create_uri.handler = &SdBrowserHandler::create_handler;
  _create_uri.user_ctx = nullptr;
}

httpd_uri_t *SdBrowserHandler::get_get_uri()
{
  return &_get_uri;
}

httpd_uri_t *SdBrowserHandler::get_post_uri()
{
  return &_create_uri;
}

esp_err_t SdBrowserHandler::create_handler(httpd_req_t *req)
{
  FILE *f = fopen("/sdcard/nuevo.str", "w");

  if (!f)
  {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error creando archivo");
    return ESP_FAIL;
  }

  fprintf(f, "Unknown");
  fclose(f);

  httpd_resp_send(req, "Archivo creado", HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

esp_err_t SdBrowserHandler::get_handler(httpd_req_t *req)
{
  DIR *dir = opendir(MOUNT_POINT);
  if (!dir)
  {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No se pudo leer la SD");
    return ESP_FAIL;
  }

  std::string html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="es">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>STRIDE Explorer</title>
        <style>
            :root {
                --primary: #2563eb;
                --primary-hover: #1d4ed8;
                --secondary: #475569;
                --secondary-hover: #334155;
                --bg: #f8fafc;
                --card-bg: #ffffff;
                --text-main: #0f172a;
                --border: #e2e8f0;
            }
            body {
                font-family: 'Segoe UI', system-ui, sans-serif;
                background-color: var(--bg);
                color: var(--text-main);
                margin: 0;
                padding: 2rem 1rem;
                display: flex;
                justify-content: center;
            }
            .container {
                background: var(--card-bg);
                padding: 2rem;
                border-radius: 16px;
                box-shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.1);
                width: 100%;
                max-width: 800px;
            }
            .header {
                display: flex;
                justify-content: space-between;
                align-items: center;
                margin-bottom: 1.5rem;
                padding-bottom: 1rem;
                border-bottom: 2px solid var(--border);
            }
            h2 {
                margin: 0;
                font-size: 1.5rem;
                display: flex;
                align-items: center;
                gap: 10px;
            }
            .btn-back {
                text-decoration: none;
                color: var(--secondary);
                font-weight: 600;
                font-size: 0.9rem;
                padding: 8px 12px;
                border-radius: 8px;
                background: var(--bg);
                transition: all 0.2s;
            }
            .btn-back:hover {
                background: #e2e8f0;
            }
            table {
                width: 100%;
                border-collapse: collapse;
            }
            th, td {
                padding: 14px 16px;
                text-align: left;
                border-bottom: 1px solid var(--border);
            }
            th {
                color: var(--secondary);
                font-weight: 600;
                font-size: 0.9rem;
                text-transform: uppercase;
                letter-spacing: 0.05em;
            }
            tr:hover td {
                background-color: #f1f5f9;
            }
            .file-name {
                display: flex;
                align-items: center;
                gap: 10px;
                font-weight: 500;
                color: var(--text-main);
            }
            .actions {
                display: flex;
                gap: 8px;
            }
            .btn-action {
                padding: 6px 14px;
                border-radius: 6px;
                text-decoration: none;
                font-size: 0.85rem;
                font-weight: 600;
                color: white;
                transition: transform 0.1s, background 0.2s;
            }
            .btn-action:hover {
                transform: translateY(-2px);
            }
            .btn-edit { background-color: var(--primary); }
            .btn-edit:hover { background-color: var(--primary-hover); }
            .btn-view { background-color: var(--secondary); }
            .btn-view:hover { background-color: var(--secondary-hover); }
            .empty-state {
                text-align: center;
                padding: 3rem;
                color: var(--secondary);
                font-style: italic;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <div class="header">
                <h2>📁 Explorador SD</h2>
                <a href="/create" class="btn-action btn-edit">+ Nuevo .str</a>
                <a href="/" class="btn-back">Volver al Inicio</a>
            </div>
            <table>
                <thead>
                    <tr>
                        <th>Nombre del Archivo</th>
                        <th style="width: 160px;">Acciones</th>
                    </tr>
                </thead>
                <tbody>
  )rawliteral";

  struct dirent *entry;
  bool hasFiles = false;

  while ((entry = readdir(dir)) != NULL)
  {
    std::string fileName = entry->d_name;

    if (fileName == "." || fileName == "..")
      continue;

    std::string ext = fileName.substr(fileName.length() - 4);
    if (ext != ".log" && ext != ".LOG" && ext != ".str" && ext != ".STR")
    {
      continue;
    }

    hasFiles = true;
    html += "<tr>";
    html += "<td><div class='file-name'>📄 " + fileName + "</div></td>";
    html += "<td class='actions'>";
    html += "<a href='/editor?file=" + fileName + "' class='btn-action btn-edit'>Editar</a>";
    html += "<a href='/view?file=" + fileName + "' class='btn-action btn-view'>Ver</a>";
    html += "</td></tr>";
  }
  closedir(dir);

  if (!hasFiles)
  {
    html += "<tr><td colspan='2' class='empty-state'>La tarjeta SD está vacía.</td></tr>";
  }

  html += R"rawliteral(
                </tbody>
            </table>
        </div>
    </body>
    </html>
  )rawliteral";

  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, html.c_str(), html.length());
  return ESP_OK;
}
