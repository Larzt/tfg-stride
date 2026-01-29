#include "wifi.h"
#include "credentials.h"
#include "config/constants.h"

static wifi_context_t *s_wifi_ctx = NULL; // contexto global del módulo
bool wifi_is_ap_mode = false;

wifi_context_t *get_wifi_context(void)
{
    return s_wifi_ctx;
}

void set_wifi_mode_ap_flag(bool ap)
{
    wifi_is_ap_mode = ap;
}

bool is_wifi_in_ap_mode()
{
    return wifi_is_ap_mode;
}

void start_sta_mode()
{
    wifi_credentials_t creds = get_wifi_credentials();

    ESP_LOGI(LWIFI, "Cambiando a modo STA...");
    set_wifi_mode_ap_flag(false);

    /* Detener antes de cambiar modo */
    esp_wifi_stop();

    wifi_config_t sta_config = {
        .sta = {
            .ssid = "",
            .password = "",
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false}},
    };

    /* Copiar credenciales */
    strncpy((char *)sta_config.sta.ssid, creds.local_ssid, sizeof(sta_config.sta.ssid));
    strncpy((char *)sta_config.sta.password, creds.local_pass, sizeof(sta_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LWIFI, "STA intentando conectar a: %s", creds.local_ssid);
}

void start_ap_mode()
{
    wifi_credentials_t creds = get_wifi_credentials();

    ESP_LOGW(LWIFI, "Cambiando a modo AP...");
    set_wifi_mode_ap_flag(true);

    /* Siempre detener WiFi antes de cambiar modo */
    esp_wifi_stop();

    /* Reiniciar contador de reintentos STA */
    wifi_context_t *ctw = get_wifi_context();
    if (ctw)
    {
        ctw->retry_count = 0;
    }

    /* Configuración del AP */
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "",
            .ssid_len = 0,
            .password = "",
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_OPEN},
    };

    /* Copiar credenciales */
    strncpy((char *)ap_config.ap.ssid, creds.ap_ssid, sizeof(ap_config.ap.ssid));
    ap_config.ap.ssid_len = strlen(creds.ap_ssid);

    strncpy((char *)ap_config.ap.password, creds.ap_pass, sizeof(ap_config.ap.password));

    /* Si hay contraseña, usar WPA2; si no, AP abierto */
    if (strlen(creds.ap_pass) > 0)
        ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LWIFI, "AP levantado: SSID=%s", creds.ap_ssid);
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base != WIFI_EVENT)
        return;

    wifi_context_t *ctw = (wifi_context_t *)arg;

    if (event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(LWIFI, "STA iniciando, intentando conectar...");
        esp_wifi_connect();
    }

    if (event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (ctw->retry_count < MAX_WIFI_CONNECT_LOCAL_RETRY)
        {
            ++ctw->retry_count;
            ESP_LOGW(LWIFI, "Reintento STA (%d/%d)...",
                     ctw->retry_count, MAX_WIFI_CONNECT_LOCAL_RETRY);
            esp_wifi_connect();
        }
        else
        {
            ESP_LOGE(LWIFI, "No se pudo conectar al wifi local.");
            start_ap_mode();
        }
    }

    if (event_id == WIFI_EVENT_STA_CONNECTED)
    {
        ESP_LOGI(LWIFI, "STA conectado a red local");
        ctw->retry_count = 0;
    }
}

static void ip_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
{
    if (event_base != IP_EVENT)
        return;

    if (event_id == IP_EVENT_STA_GOT_IP)
    {
        ESP_LOGI(LWIFI, "IP obtenida, apagando AP y quedando en STA.");

        // esp_wifi_stop();
        // esp_wifi_set_mode(WIFI_MODE_STA);
        // esp_wifi_start();
    }
}

void init_wifi()
{
    s_wifi_ctx = calloc(1, sizeof(wifi_context_t));

    if (!s_wifi_ctx)
    {
        ESP_LOGE(LWIFI, "No memory for wifi context");
        return;
    }

    // Inicializar el driver WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID,
        &wifi_event_handler, s_wifi_ctx, NULL);

    esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP,
        &ip_event_handler, s_wifi_ctx, NULL);

    wifi_credentials_t creds = get_wifi_credentials();
    bool has_local = (creds.local_ssid[0] != '\0');

    if (has_local)
    {
        ESP_LOGI(LWIFI, "Credenciales locales encontradas: iniciando STA");
        start_sta_mode();
    }
    else
    {
        ESP_LOGW(LWIFI, "Sin credenciales locales: iniciando AP de configuración");
        start_ap_mode();
    }
}
