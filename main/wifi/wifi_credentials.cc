#include <stdio.h>
#include <string.h>
#include "credentials.h"
#include "wifi_credentials.h"

#include "esp_log.h"
#include "cJSON.h"

#include "nvs_flash.h"
#include "nvs.h"

// El archivo fue embebido mediante CMake (EMBED_FILES)
extern const char config_json_start[] asm("_binary_config_json_start");
extern const char config_json_end[] asm("_binary_config_json_end");

wifi_credentials_t get_wifi_credentials(void)
{
  wifi_credentials_t creds;

  strcpy(creds.ap_ssid, DEFAULT_AP_SSID);
  strcpy(creds.ap_pass, DEFAULT_AP_PASS);
  strcpy(creds.local_ssid, DEFAULT_LOCAL_SSID);
  strcpy(creds.local_pass, DEFAULT_LOCAL_PASS);

  // Leer archivo embebido → copiar a buffer
  size_t json_size = config_json_end - config_json_start;

  if (json_size == 0)
  {
    ESP_LOGW("WIFI CREDS: ", "config.json está vacío → usando defaults");
    return creds;
  }

  char *buffer = static_cast<char *>(malloc(json_size + 1));
  if (!buffer)
  {
    ESP_LOGE("WIFI CREDS: ", "No hay memoria para copiar config.json");
    return creds;
  }

  memcpy(buffer, config_json_start, json_size);
  buffer[json_size] = '\0';

  // Parsear JSON
  cJSON *root = cJSON_Parse(buffer);
  free(buffer);

  if (!root)
  {
    const char *err = cJSON_GetErrorPtr();
    ESP_LOGW("WIFI CREDS: ", "Error parseando config.json: %s", err ? err : "desconocido");
    return creds;
  }

  // Obtener objeto ESPAP
  cJSON *esp_ap = cJSON_GetObjectItemCaseSensitive(root, "ESPAP");
  if (esp_ap)
  {
    cJSON *ssid = cJSON_GetObjectItemCaseSensitive(esp_ap, "SSID");
    cJSON *pass = cJSON_GetObjectItemCaseSensitive(esp_ap, "PASS");

    if (cJSON_IsString(ssid) && ssid->valuestring)
    {
      strcpy(creds.ap_ssid, ssid->valuestring);
    }

    if (cJSON_IsString(pass) && pass->valuestring)
    {
      strcpy(creds.ap_pass, pass->valuestring);
    }
  }

  // Obtener objeto LOCAL
  cJSON *local = cJSON_GetObjectItemCaseSensitive(root, "LOCAL");
  if (local)
  {
    cJSON *ssid = cJSON_GetObjectItemCaseSensitive(local, "SSID");
    cJSON *pass = cJSON_GetObjectItemCaseSensitive(local, "PASS");

    if (cJSON_IsString(ssid) && ssid->valuestring)
    {
      strcpy(creds.local_ssid, ssid->valuestring);
    }

    if (cJSON_IsString(pass) && pass->valuestring)
    {
      strcpy(creds.local_pass, pass->valuestring);
    }
  }

  cJSON_Delete(root);

  ESP_LOGI("WIFI CREDS: ", "Se ha cargado config.json correctamente.");

  return creds;
}

void save_wifi_credentials(const char *ssid, const char *pass)
{
  nvs_handle_t nvs_handle;
  esp_err_t err = nvs_open("wifi_creds", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK)
  {
    ESP_LOGE("WIFI CREDS: ", "No se pudo abrir NVS: %s", esp_err_to_name(err));
    return;
  }

  nvs_set_str(nvs_handle, "ssid", ssid);
  nvs_set_str(nvs_handle, "pass", pass);

  err = nvs_commit(nvs_handle);
  if (err != ESP_OK)
  {
    ESP_LOGE("WIFI CREDS: ", "Error al guardar credenciales: %s", esp_err_to_name(err));
  }
  else
  {
    ESP_LOGI("WIFI CREDS: ", "Credenciales guardadas: SSID=%s", ssid);
  }

  nvs_close(nvs_handle);
}
