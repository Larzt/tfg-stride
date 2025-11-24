#pragma once

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

typedef struct
{
  int retry_count;
} wifi_context_t;

// Inicializa el ESP32 en modo AP + STA
void wifi_init_softap_sta(void);
