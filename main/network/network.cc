#include "network.h"
#include "display.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>

const char *Network::TAG = "Network";
#define WIFI_PIN_27 GPIO_NUM_27

int Network::s_retry_num = 0;
WifiMode Network::_mode = WifiMode::AP;
OutputPin Network::wifi_led(WIFI_PIN_27);

Network::Network() {}

// --- Event Handler ---
void Network::event_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        _mode = AP;
        wifi_led.off();
        if (s_retry_num < 5)
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
        wifi_led.on();
        s_retry_num = 0;
        _mode = STA;
    }
}

// --- Inicialización ---
void Network::init()
{
    wifi_led.init();
    std::string ssid, password;

    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Inicializar TCP/IP
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Registrar eventos
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    // Cargar credenciales guardadas
    WifiMode start_mode = AP;
    wifi_config_t sta_config = {};
    if (load_wifi_credentials(ssid, password))
    {
        ESP_LOGI(TAG, "Cargando credenciales desde NVS: %s", ssid.c_str());
        strlcpy((char *)sta_config.sta.ssid, ssid.c_str(), sizeof(sta_config.sta.ssid));
        strlcpy((char *)sta_config.sta.password, password.c_str(), sizeof(sta_config.sta.password));
        start_mode = STA;
    }

    // !TODO: Hay que cambiar esto, la configuracion del wifi AP esta hardcode
    // Configuración AP por defecto
    wifi_config_t ap_config = {};
    strlcpy((char *)ap_config.ap.ssid, "ESP32-Config", sizeof(ap_config.ap.ssid));
    strlcpy((char *)ap_config.ap.password, "12345678", sizeof(ap_config.ap.password));
    ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap_config.ap.max_connection = 4;
    ap_config.ap.channel = 1;

    // Arrancar en APSTA
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (netif != NULL)
    {
        esp_netif_get_ip_info(netif, &ip_info);
        char ip_str[16];
        snprintf(ip_str, sizeof(ip_str), IPSTR, IP2STR(&ip_info.ip));

        auto &display = Display::Instance();
        display.setMode(DisplayState::CONFIG_AP);
        display.showAPInfo(ip_str);
    }

    _mode = start_mode;
    ESP_LOGI(TAG, "Wi-Fi iniciado en modo %s", (_mode == STA) ? "STA" : "AP");
}

bool Network::load_wifi_credentials(std::string &ssid, std::string &password)
{
    nvs_handle_t my_handle;
    if (nvs_open("wifi", NVS_READONLY, &my_handle) != ESP_OK)
        return false;

    size_t ssid_len = 0, pass_len = 0;
    if (nvs_get_str(my_handle, "ssid", NULL, &ssid_len) != ESP_OK)
    {
        nvs_close(my_handle);
        return false;
    }
    if (nvs_get_str(my_handle, "password", NULL, &pass_len) != ESP_OK)
    {
        nvs_close(my_handle);
        return false;
    }

    char *ssid_buf = new char[ssid_len];
    char *pass_buf = new char[pass_len];

    nvs_get_str(my_handle, "ssid", ssid_buf, &ssid_len);
    nvs_get_str(my_handle, "password", pass_buf, &pass_len);

    ssid = ssid_buf;
    password = pass_buf;

    delete[] ssid_buf;
    delete[] pass_buf;
    nvs_close(my_handle);
    return true;
}

void Network::save_wifi_credentials(const std::string &ssid, const std::string &password)
{
    nvs_handle_t my_handle;
    ESP_ERROR_CHECK(nvs_open("wifi", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, "ssid", ssid.c_str()));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, "password", password.c_str()));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}
