#pragma once

#include <string>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

// pins
#include "output.h"

enum WifiMode
{
  AP,
  STA
};

class WifiHandler
{
public:
  WifiHandler();
  void init();
  static bool load_wifi_credentials(std::string &ssid, std::string &password);
  static void save_wifi_credentials(const std::string &ssid, const std::string &password);

  static OutputPin wifi_led;
  static inline WifiMode current_mode() { return _mode; }

private:
  static void
  event_handler(void *arg, esp_event_base_t event_base,
                int32_t event_id, void *event_data);

  static const char *TAG;
  static int s_retry_num;
  static WifiMode _mode;
};
