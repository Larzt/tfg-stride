#pragma once
#include "esp_wifi.h"
#include "esp_event.h"

#include "stride_logger.h"
#include "blackboard.h"


enum NetworkMode
{
  Access,
  Station,
};

class Network
{
public:
  Network();

  void connect();
  static bool load_net_credentials(const std::string &ssid, const std::string &pass);
  static bool save_net_credentials(const std::string &ssid, const std::string &pass);

  // static Output led;

  ~Network();

private:
  event_handler(void *arg, esp_event_base_t event_base,
                int32_t event_id, void *event_data);
};
