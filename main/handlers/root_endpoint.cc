#include <string>
#include "utility.h"
#include "root_endpoint.h"
#include "esp_log.h"

RootHandler::RootHandler()
{
  _uri.uri = "/";
  _uri.method = HTTP_GET;
  _uri.handler = &RootHandler::handler;
  _uri.user_ctx = nullptr;
}

httpd_uri_t *RootHandler::get_get_uri()
{
  return &_uri;
}

esp_err_t RootHandler::handler(httpd_req_t *req)
{
  // HTML, CSS y un poco de estructura para una Landing Page moderna
  std::string html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="es">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP32 Dashboard</title>
        <style>
            :root {
                --primary: #2563eb;
                --primary-hover: #1d4ed8;
                --bg: #f8fafc;
                --card-bg: #ffffff;
                --text-main: #0f172a;
                --text-muted: #64748b;
            }
            body {
                font-family: 'Segoe UI', system-ui, sans-serif;
                background-color: var(--bg);
                color: var(--text-main);
                margin: 0;
                display: flex;
                justify-content: center;
                align-items: center;
                min-height: 100vh;
            }
            .card {
                background: var(--card-bg);
                padding: 2.5rem;
                border-radius: 16px;
                box-shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.1), 0 8px 10px -6px rgba(0, 0, 0, 0.1);
                text-align: center;
                max-width: 400px;
                width: 90%;
            }
            .icon {
                font-size: 3rem;
                margin-bottom: 1rem;
            }
            h1 {
                margin: 0 0 0.5rem 0;
                font-size: 1.75rem;
            }
            p {
                color: var(--text-muted);
                margin-bottom: 2rem;
                line-height: 1.5;
            }
            .status-badge {
                display: inline-flex;
                align-items: center;
                gap: 6px;
                background: #dcfce7;
                color: #166534;
                padding: 4px 12px;
                border-radius: 999px;
                font-size: 0.875rem;
                font-weight: 600;
                margin-bottom: 1.5rem;
            }
            .status-dot {
                width: 8px;
                height: 8px;
                background-color: #22c55e;
                border-radius: 50%;
                box-shadow: 0 0 8px #22c55e;
            }
            .btn {
                display: block;
                width: 100%;
                box-sizing: border-box;
                padding: 12px 20px;
                color: white;
                background: var(--primary);
                text-decoration: none;
                border-radius: 8px;
                font-weight: 600;
                transition: all 0.2s ease;
            }
            .btn:hover {
                background: var(--primary-hover);
                transform: translateY(-2px);
            }
        </style>
    </head>
    <body>
        <div class="card">
            <h1>Sistema Stride</h1>

            <div class="status-badge">
                <div class="status-dot"></div>
                En línea y operando
            </div>

            <p>Bienvenido al panel de control principal. Desde aquí puedes gestionar los programas de la tarjeta SD.</p>
            <a href="browser" class="btn">Entrar</a>
        </div>
    </body>
    </html>
  )rawliteral";

  // Cambiamos el tipo de respuesta a HTML
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, html.c_str(), html.length());

  ESP_LOGI("ROOT_ENDPOINT", "Landing page servida con éxito");

  return ESP_OK;
}
