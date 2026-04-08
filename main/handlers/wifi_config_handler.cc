#include "wifi_config_handler.h"
#include "WifiHandler.h"

#include <fstream>
#include <sstream>
#include <string.h>
#include "esp_log.h"

WifiConfigHandler::WifiConfigHandler()
{
  // GET
  _get_uri.uri = "/wifi";
  _get_uri.method = HTTP_GET;
  _get_uri.handler = &WifiConfigHandler::get_handler;
  _get_uri.user_ctx = nullptr;

  // POST
  _post_uri.uri = "/wifi";
  _post_uri.method = HTTP_POST;
  _post_uri.handler = &WifiConfigHandler::post_handler;
  _post_uri.user_ctx = nullptr;
}

httpd_uri_t *WifiConfigHandler::get_get_uri()
{
  return &_get_uri;
}

httpd_uri_t *WifiConfigHandler::get_post_uri()
{
  return &_post_uri;
}

esp_err_t WifiConfigHandler::get_handler(httpd_req_t *req)
{
    std::string ssid = "";
    std::string password = "";
    std::string message = "";

    // Cargar credenciales desde NVS
    WifiHandler::load_wifi_credentials(ssid, password);

    // Detectar mensaje ?saved=1
    char query[64];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK)
    {
        if (strstr(query, "saved=1"))
            message = "Guardado correctamente";
    }

    std::stringstream html;
    html << R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Configuración WiFi</title>
<style>
    * { box-sizing: border-box; }
    body { font-family: Arial; background: #f2f4f8; display:flex; justify-content:center; align-items:center; height:100vh; margin:0; }
    .card { background:white; padding:25px; border-radius:10px; box-shadow:0 4px 10px rgba(0,0,0,0.1); width:320px; }
    h2 { text-align:center; margin-bottom:20px; }
    label { font-size:14px; }
    input { width:100%; padding:10px; margin:8px 0 12px 0; border-radius:6px; border:1px solid #ccc; display:block; }
    button { margin-bottom:10px; width:100%; padding:10px; background:#007BFF; color:white; border:none; border-radius:6px; cursor:pointer; font-size:15px; }
    button:hover { background:#0056b3; }
    .msg { text-align:center; color:green; margin-top:10px; }
</style>
<script>
function togglePassword() {
    var x = document.getElementById("pwd");
    x.type = (x.type === "password") ? "text" : "password";
}
</script>
</head>
<body>
<div class="card">
<h2>WiFi Setup</h2>
<form method="POST" action="/wifi">
<label>SSID</label>
<input name="ssid" value=")rawliteral";

    html << ssid;

    html << R"rawliteral(">
<label>Password</label>
<input type="password" id="pwd" name="password" value=")rawliteral";

    html << password;

    html << R"rawliteral(">
<button type="button" onclick="togglePassword()">Mostrar / Ocultar</button>
<button type="submit">Guardar</button>
</form>
<div class="msg">)rawliteral";

    html << message;

    html << R"rawliteral(</div>
</div>
</body>
</html>
)rawliteral";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html.str().c_str(), html.str().length());

    return ESP_OK;
}

esp_err_t WifiConfigHandler::post_handler(httpd_req_t *req)
{
    char buf[200];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret <= 0) return ESP_FAIL;

    std::string body(buf, ret);
    std::string ssid, password;

    // Parseo básico
    size_t ssid_pos = body.find("ssid=");
    size_t pass_pos = body.find("password=");

    if (ssid_pos != std::string::npos)
    {
        size_t end = body.find("&", ssid_pos);
        ssid = body.substr(ssid_pos + 5, end - (ssid_pos + 5));
    }
    if (pass_pos != std::string::npos)
    {
        password = body.substr(pass_pos + 9);
    }

    // Guardar credenciales en NVS
    WifiHandler::save_wifi_credentials(ssid, password);

    // Redirigir a GET con mensaje
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/wifi?saved=1");
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}
