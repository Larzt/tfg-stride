#pragma once

#define EXAMPLE_MAX_STA_CONN 4

typedef struct
{
  char ap_ssid[32];
  char ap_pass[64];

  char local_ssid[32];
  char local_pass[64];
} wifi_credentials_t;

wifi_credentials_t get_wifi_credentials(void);
void save_wifi_credentials(const char *ssid, const char *pass);
