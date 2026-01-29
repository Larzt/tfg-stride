#ifndef WIFI_H
#define WIFI_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

extern bool wifi_is_ap_mode;

typedef struct
{
  int retry_count;
} wifi_context_t;

// Inicializa el ESP32 en modo AP + STA
void init_wifi(void);

void set_wifi_mode_ap_flag(bool ap);

bool is_wifi_in_ap_mode();

void start_sta_mode();

#endif
