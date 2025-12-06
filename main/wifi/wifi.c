#include "wifi.h"
#include "credentials.h"
// #include "config/credentials.h"
#include "config/constants.h"

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    wifi_context_t *ctw = (wifi_context_t *)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(LWIFI, "STA iniciando, intentando conectar...");
        esp_wifi_connect();
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        // Limitar las reconecciones para evitar que colapse el AP
        if (ctw->retry_count < MAX_WIFI_CONNECT_LOCAL_RETRY)
        {
            ++ctw->retry_count;
            ESP_LOGW(LWIFI, "Reintento STA (%d/%d)...", ctw->retry_count, MAX_WIFI_CONNECT_LOCAL_RETRY);
            esp_wifi_connect();
        }
        else
        {
            ESP_LOGE(LWIFI, "No se pudo conectar al wifi local.");
        }
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        ESP_LOGI(LWIFI, "STA conectado a red local");
        ctw->retry_count = 0;
    }
}

void wifi_init_softap_sta(void)
{
    // Crear interfaces AP y STA
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_context_t *wifi_ctx = calloc(1, sizeof(wifi_context_t));

    // Registrar eventos
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        wifi_ctx,
        NULL));

    // Obtener las credenciales
    wifi_credentials_t creds = get_wifi_credentials();

    wifi_config_t wifi_ap_config = {0};
    wifi_config_t wifi_sta_config = {0};

    // Config AP
    strncpy((char *)wifi_ap_config.ap.ssid, creds.ap_ssid, sizeof(wifi_ap_config.ap.ssid));
    strncpy((char *)wifi_ap_config.ap.password, creds.ap_pass, sizeof(wifi_ap_config.ap.password));

    wifi_ap_config.ap.ssid_len = strlen(creds.ap_ssid);
    wifi_ap_config.ap.channel = 1;
    wifi_ap_config.ap.max_connection = EXAMPLE_MAX_STA_CONN;
    wifi_ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    // Config STA
    strncpy((char *)wifi_sta_config.sta.ssid, creds.local_ssid, sizeof(wifi_sta_config.sta.ssid));
    strncpy((char *)wifi_sta_config.sta.password, creds.local_pass, sizeof(wifi_sta_config.sta.password));

    wifi_sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    // Modo AP+STA
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config));

    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(LWIFI, "Modo AP+STA iniciado con AP: %s y STA: %s", creds.ap_ssid, creds.local_ssid);
}
