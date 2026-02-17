#include "WifiHandler.h"
#include "wifi_credentials.h"

#include "nvs_flash.h"
#include <string>

const char *WifiHandler::TAG = "WifiHandler";
#define WIFI_PIN_27 GPIO_NUM_27

int WifiHandler::s_retry_num = 0;
OutputPin WifiHandler::wifi_led(WIFI_PIN_27);

WifiHandler::WifiHandler() {}

void WifiHandler::event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    wifi_led.turn(false);
    if (s_retry_num < 10)
    {
      esp_wifi_connect();
      s_retry_num++;
      ESP_LOGI(TAG, "Reintentando conexión al AP...");
    }
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "IP Obtenida: " IPSTR, IP2STR(&event->ip_info.ip));
    wifi_led.turn(true);
    s_retry_num = 0;
  }
}

void WifiHandler::init()
{
  wifi_credentials_t creds = get_wifi_credentials();
  wifi_led.init();

  // 1. Inicializar NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // 2. Inicializar TCP/IP
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // 3. Registrar eventos
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

  // 4. Configurar Dual Mode
  wifi_config_t wifi_config = {};

  // Configuración AP - Eliminado .c_str() ya que son char arrays
  // Usamos strlcpy por seguridad (garantiza el null-terminator)
  strlcpy((char *)wifi_config.ap.ssid, creds.ap_ssid, sizeof(wifi_config.ap.ssid));
  strlcpy((char *)wifi_config.ap.password, creds.ap_pass, sizeof(wifi_config.ap.password));

  wifi_config.ap.ssid_len = strlen(creds.ap_ssid);
  wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
  wifi_config.ap.max_connection = EXAMPLE_MAX_STA_CONN;
  wifi_config.ap.channel = 1;

  // Configuración STA - Cambiado sta_ssid por local_ssid según tu struct
  wifi_config_t sta_config = {};
  strlcpy((char *)sta_config.sta.ssid, creds.local_ssid, sizeof(sta_config.sta.ssid));
  strlcpy((char *)sta_config.sta.password, creds.local_pass, sizeof(sta_config.sta.password));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));

  ESP_LOGI(TAG, "Iniciando Wi-Fi Dual Mode...");
  ESP_ERROR_CHECK(esp_wifi_start());
}
