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
  char query[128];

  if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK)
  {
    char filename[32];
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
    <html>
    <head>
        <title>SD Editor</title>
        <meta charset="UTF-8">
    </head>
    <body>
        <h2>Editor de programa en SD</h2>
        <form method="POST" action="/editor">
            <textarea name="content" rows="20" cols="80">)rawliteral";

  html += file_content.str();

  html += R"rawliteral(</textarea><br><br>
            <button type="submit">Guardar</button>
        </form>
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
  int received = 0;

  std::string body;
  body.resize(total_len);

  while (received < total_len)
  {
    int r = httpd_req_recv(req, &body[received], total_len - received);
    if (r <= 0)
      return ESP_FAIL;
    received += r;
  }

  // Extraer solo el contenido del textarea
  std::string content = body;
  size_t pos = content.find("content=");
  if (pos != std::string::npos)
  {
    content = content.substr(pos + 8);
  }

  content = url_decode(content);

  FILE *f = fopen(FILE_PATH, "w");
  if (f == NULL)
  {
    ESP_LOGE("SD_EDITOR", "No se pudo abrir archivo para escribir");
    return ESP_FAIL;
  }

  fprintf(f, "%s", content.c_str());
  fclose(f);

  ESP_LOGI("SD_EDITOR", "Archivo guardado correctamente");

  httpd_resp_sendstr(req, "Guardado correctamente");
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
