#include <string>
#include <sstream>
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "sd_editor_endpoint.h"

#define FILE_PATH "/sdcard/program.str"

SdEditorHandler::SdEditorHandler()
{
  _get_uri.uri = "/editor";
  _get_uri.method = HTTP_GET;
  _get_uri.handler = &SdEditorHandler::get_handler;
  _get_uri.user_ctx = nullptr;

  _post_uri.uri = "/editor";
  _post_uri.method = HTTP_POST;
  _post_uri.handler = &SdEditorHandler::post_handler;
  _post_uri.user_ctx = nullptr;
}

httpd_uri_t *SdEditorHandler::get_get_uri()
{
  return &_get_uri;
}

httpd_uri_t *SdEditorHandler::get_post_uri()
{
  return &_post_uri;
}

esp_err_t SdEditorHandler::get_handler(httpd_req_t *req)
{
  char filepath[64] = FILE_PATH;
  char filename[32] = "program.str"; // Nombre por defecto
  char query[128];

  if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK)
  {
    if (httpd_query_key_value(query, "file", filename, sizeof(filename)) == ESP_OK)
    {
      snprintf(filepath, sizeof(filepath), "/sdcard/%s", filename);
    }
  }

  std::stringstream file_content;
  FILE *f = fopen(filepath, "r");

  if (f != NULL)
  {
    char line[256];
    while (fgets(line, sizeof(line), f))
    {
      file_content << line;
    }
    fclose(f);
  }

  std::string html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="es">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Editor de Código</title>
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
            .btn-back:hover { background: #e2e8f0; }
            form {
                display: flex;
                flex-direction: column;
                flex-grow: 1;
                gap: 1.5rem;
            }
            textarea {
                width: 100%;
                height: 60vh;
                padding: 1rem;
                font-family: 'Consolas', 'Courier New', monospace;
                font-size: 1rem;
                line-height: 1.5;
                color: var(--text-main);
                background-color: #f1f5f9;
                border: 1px solid var(--border);
                border-radius: 8px;
                box-sizing: border-box;
                resize: vertical;
            }
            textarea:focus {
                outline: none;
                border-color: var(--primary);
                box-shadow: 0 0 0 3px rgba(37, 99, 235, 0.2);
            }
            .footer {
                display: flex;
                justify-content: flex-end;
            }
            .btn-save {
                background-color: var(--primary);
                color: white;
                border: none;
                padding: 12px 24px;
                font-size: 1rem;
                font-weight: 600;
                border-radius: 8px;
                cursor: pointer;
                transition: all 0.2s;
            }
            .btn-save:hover {
                background-color: var(--primary-hover);
                transform: translateY(-2px);
            }
        </style>
    </head>
    <body>
        <div class="container">
            <div class="header">
  )rawliteral";

  html += "<h2>📝 Editando: " + std::string(filename) + "</h2>";
  html += "<a href=\"/browser\" class=\"btn-back\">Volver al Explorador</a></div>";

  html += "<form method=\"POST\" action=\"/editor?file=" + std::string(filename) + "\">";
  html += R"rawliteral(<textarea name="content" spellcheck="false">)rawliteral";

  html += file_content.str();

  html += R"rawliteral(</textarea>
                <div class="footer">
                    <button type="submit" class="btn-save">Guardar Cambios</button>
                </div>
            </form>
        </div>
    </body>
    </html>
  )rawliteral";

  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, html.c_str(), html.length());

  return ESP_OK;
}

esp_err_t SdEditorHandler::post_handler(httpd_req_t *req)
{
  int total_len = req->content_len;

  // Limitar el tamaño máximo del archivo por seguridad (16KB)
  if (total_len > 16384)
  {
    ESP_LOGE("SD_EDITOR", "Archivo demasiado grande");
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "File too large");
    return ESP_FAIL;
  }

  // Usamos un solo buffer para todo
  std::string body;
  body.resize(total_len);

  int received = 0;
  while (received < total_len)
  {
    int r = httpd_req_recv(req, &body[received], total_len - received);
    if (r <= 0)
    {
      if (r == HTTPD_SOCK_ERR_TIMEOUT)
        continue; // Si hay timeout, reintentar
      return ESP_FAIL;
    }
    received += r;
  }

  size_t pos = body.find("content=");
  if (pos == std::string::npos)
  {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid form data");
    return ESP_FAIL;
  }

  std::string content_url_encoded = body.substr(pos + 8);

  // Liberar el buffer gigante inicial para recuperar RAM antes de decodificar
  body.clear();
  body.shrink_to_fit();

  std::string final_content = url_decode(content_url_encoded);

  FILE *f = fopen(FILE_PATH, "w");
  if (f == NULL)
  {
    ESP_LOGE("SD_EDITOR", "No se pudo abrir archivo para escribir");
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "SD Write Error");
    return ESP_FAIL;
  }

  fprintf(f, "%s", final_content.c_str());
  fclose(f);

  ESP_LOGI("SD_EDITOR", "Archivo guardado correctamente (%d bytes)", final_content.length());

  // Redirigir de vuelta al editor para no quedarse en una página en blanco
  httpd_resp_set_status(req, "303 See Other");
  httpd_resp_set_hdr(req, "Location", "/editor");
  httpd_resp_send(req, NULL, 0);

  return ESP_OK;
}

std::string SdEditorHandler::url_decode(const std::string &src)
{
  std::string ret;
  char ch;
  int i, ii;
  for (i = 0; i < src.length(); i++)
  {
    if (src[i] == '%')
    {
      sscanf(src.substr(i + 1, 2).c_str(), "%x", &ii);
      ch = static_cast<char>(ii);
      ret += ch;
      i += 2;
    }
    else if (src[i] == '+')
    {
      ret += ' ';
    }
    else
    {
      ret += src[i];
    }
  }
  return ret;
}
