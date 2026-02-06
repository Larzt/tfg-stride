#include <string>
#include "utility.h"
#include "config_endpoint.h"
#include "wifi_credentials.h"

ConfigHandler::ConfigHandler()
{
  _uri.uri = "/config";
  _uri.method = HTTP_GET;
  _uri.handler = &ConfigHandler::handler;
  _uri.user_ctx = nullptr;
}

httpd_uri_t *ConfigHandler::get_uri()
{
  return &_uri;
}

esp_err_t ConfigHandler::handler(httpd_req_t *req)
{
  wifi_credentials_t creds = get_wifi_credentials();

  std::string resp = "{" + kENDL;
  resp += "  \"ESP_AP\": {" + kENDL;
  resp += "    \"SSID\": \"" + std::string(creds.ap_ssid) + "\"," + kENDL;
  resp += "    \"PASS\": \"" + std::string(creds.ap_pass) + "\"" + kENDL;
  resp += "  }," + kENDL;
  resp += "  \"LOCAL\": {" + kENDL;
  resp += "    \"SSID\": \"" + std::string(creds.local_ssid) + "\"," + kENDL;
  resp += "    \"PASS\": \"" + std::string(creds.local_pass) + "\"" + kENDL;
  resp += "  }" + kENDL;
  resp += "}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), resp.length());
  return ESP_OK;
}
