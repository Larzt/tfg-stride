#include <string>
#include "utility.h"
#include "config_endpoint.h"
#include "network.h"

ConfigHandler::ConfigHandler()
{
  _uri.uri = "/config";
  _uri.method = HTTP_GET;
  _uri.handler = &ConfigHandler::handler;
  _uri.user_ctx = nullptr;
}

httpd_uri_t *ConfigHandler::get_get_uri()
{
  return &_uri;
}

esp_err_t ConfigHandler::handler(httpd_req_t *req)
{
  std::string ssid = "";
  std::string password = "";

  Network::load_wifi_credentials(ssid, password);

  std::string resp = "{" + kENDL;
  resp += "  \"ESP_AP\": {" + kENDL;
  resp += "    \"SSID\": \"" + std::string(ssid) + "\"," + kENDL;
  resp += "    \"PASS\": \"" + std::string(password) + "\"" + kENDL;
  resp += "  }," + kENDL;
  resp += "  \"LOCAL\": {" + kENDL;
  resp += "    \"SSID\": \"" + std::string(ssid) + "\"," + kENDL;
  resp += "    \"PASS\": \"" + std::string(password) + "\"" + kENDL;
  resp += "  }" + kENDL;
  resp += "}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), resp.length());
  return ESP_OK;
}
