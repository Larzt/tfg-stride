#include "sd_reader_endpoint.h"
#include <stdio.h>
#include <sstream>
#include "esp_log.h"

#define SD_BASE_PATH "/sdcard"

SdReaderHandler::SdReaderHandler()
{
  _get_uri.uri = "/view";
  _get_uri.method = HTTP_GET;
  _get_uri.handler = &SdReaderHandler::get_handler;
  _get_uri.user_ctx = nullptr;
}

httpd_uri_t *SdReaderHandler::get_get_uri()
{
  return &_get_uri;
}

esp_err_t SdReaderHandler::get_handler(httpd_req_t *req)
{
  char query[128];
  char filepath[128];
  char filename[64] = "Desconocido";
  bool file_found = false;

  if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK)
  {
    if (httpd_query_key_value(query, "file", filename, sizeof(filename)) == ESP_OK)
    {
      snprintf(filepath, sizeof(filepath), "%s/%s", SD_BASE_PATH, filename);
      file_found = true;
    }
  }

  if (!file_found)
  {
    ESP_LOGW("SD_READER", "Petición sin parámetro 'file'");
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Falta el parámetro 'file'");
    return ESP_FAIL;
  }

  FILE *f = fopen(filepath, "r");
  if (!f)
  {
    ESP_LOGE("SD_READER", "Error al abrir: %s", filepath);
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Archivo no encontrado en SD");
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "text/html; charset=utf-8");

  std::string html_head = R"rawliteral(
    <!DOCTYPE html>
    <html lang="es">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Visor de Código</title>
        <style>
            :root {
                --primary: #2563eb;
                --primary-hover: #1d4ed8;
                --secondary: #475569;
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
                min-height: 100vh;
                box-sizing: border-box;
            }
            .container {
                background: var(--card-bg);
                padding: 2rem;
                border-radius: 16px;
                box-shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.1);
                width: 100%;
                max-width: 900px;
                display: flex;
                flex-direction: column;
            }
            .header {
                display: flex;
                justify-content: space-between;
                align-items: center;
                margin-bottom: 1.5rem;
                padding-bottom: 1rem;
                border-bottom: 2px solid var(--border);
                flex-wrap: wrap;
                gap: 1rem;
            }
            h2 {
                margin: 0;
                font-size: 1.5rem;
                display: flex;
                align-items: center;
                gap: 10px;
            }
            .btn-group {
                display: flex;
                gap: 10px;
            }
            .btn {
                text-decoration: none;
                font-weight: 600;
                font-size: 0.9rem;
                padding: 8px 16px;
                border-radius: 8px;
                transition: all 0.2s;
            }
            .btn-back { color: var(--secondary); background: var(--bg); }
            .btn-back:hover { background: #e2e8f0; }
            .btn-edit { color: white; background: var(--primary); }
            .btn-edit:hover { background: var(--primary-hover); transform: translateY(-2px); }
            .code-box {
                background-color: #1e293b;
                color: #f8fafc;
                padding: 1.5rem;
                border-radius: 8px;
                overflow-x: auto;
                font-family: 'Consolas', 'Courier New', monospace;
                font-size: 0.95rem;
                line-height: 1.6;
                box-shadow: inset 0 2px 4px 0 rgba(0, 0, 0, 0.06);
                margin: 0;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <div class="header">
  )rawliteral";

  html_head += "<h2>👁️ Viendo: " + std::string(filename) + "</h2>";
  html_head += "<div class=\"btn-group\">";
  html_head += "<a href=\"/browser\" class=\"btn btn-back\">Explorador</a>";
  html_head += "<a href=\"/editor?file=" + std::string(filename) + "\" class=\"btn btn-edit\">Editar Archivo</a>";
  html_head += "</div></div><pre class=\"code-box\"><code>";

  httpd_resp_send_chunk(req, html_head.c_str(), html_head.length());

  char line[256];
  while (fgets(line, sizeof(line), f))
  {
    std::string safe_line = "";

    for (char c : std::string(line))
    {
      if (c == '<')
        safe_line += "&lt;";
      else if (c == '>')
        safe_line += "&gt;";
      else
        safe_line += c;
    }
    httpd_resp_send_chunk(req, safe_line.c_str(), safe_line.length());
  }
  fclose(f);

  std::string html_tail = "</code></pre></div></body></html>";
  httpd_resp_send_chunk(req, html_tail.c_str(), html_tail.length());

  httpd_resp_sendstr_chunk(req, NULL);

  ESP_LOGI("SD_READER", "Archivo servido con diseño: %s", filepath);
  return ESP_OK;
}
