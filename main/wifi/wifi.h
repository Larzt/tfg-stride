#pragma once

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

// Inicializa el ESP32 en modo AP + STA
void wifi_init_softap_sta(void);
