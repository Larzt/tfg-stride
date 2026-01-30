#pragma once

#include <string>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

class WifiHandler
{
public:
  WifiHandler();
  void init(const std::string &ap_ssid, const std::string &ap_pass,
            const std::string &sta_ssid, const std::string &sta_pass);

private:
  static void event_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data);

  static const char *TAG;
  static int s_retry_num;
};
