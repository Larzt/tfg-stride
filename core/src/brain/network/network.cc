#include "network.h"

#include "nvs_flash.h"
#include <string>

Network::Network() {}

Network::connect()
{
  // output.init()

  std::string ssid, password;

  // NVS Initialization
  esp_err_t res = nvs_flash_init();
  if (res == ESP_ERR_NVS_NO_FREE_PAGES || res == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    res = nvs_flash_init();
  }
  ESP_ERROR_CHECK(res);

  // Network stack Initialization
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_create_default_wifi_sta();
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // Events register
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

  // Load STA saved credentials
  wifi_config_t sta_config = {};
  bool has_sta = false;
  if (load_net_credentials(ssid, password))
  {
    StrideLogger::Log(StrideSubsystem::Network, "Loading credentials: %s", ssid.c_str());
    strlcpy((char *)sta_config.sta.ssid, ssid.c_str(), sizeof(sta_config.sta.ssid));
    strlcpy((char *)sta_config.sta.password, password.c_str(), sizeof(sta_config.sta.password));
    has_sta = true;
  }

  // Load AP saved credentials
  wifi_config_t ap_config = {};
  strlcpy((char *)ap_config.ap.ssid, Blackboard::AccessSSID, sizeof(ap_config.ap.ssid));
  strlcpy((char *)ap_config.ap.password, Blackboard::AccessPASS, sizeof(ap_config.ap.password));
  ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
  ap_config.ap.max_connection = Blackboard::MaxNetConnections;
  ap_config.ap.channel = 1;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  // Network start
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));

  if (has_sta)
  {
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
  }

  ESP_ERROR_CHECK(esp_wifi_start());

  if (has_sta)
  {
    ESP_ERROR_CHECK(esp_wifi_connect());
  }

  Blackboard::NetworkMode = has_sta ? NetworkMode::Station : NetworkMode::Access;
}
