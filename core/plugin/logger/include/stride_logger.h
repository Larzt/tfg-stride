#pragma once

#include "esp_log.h"

enum StrideSubsystem
{
  None,
  Network,
  Server,
};

class StrideLogger
{

public:
  template <typename... Args>
  static void Log(StrideSubsystem sub, const char *format, Args... args)
  {
    const char *tag = get_tag(sub);
    esp_log_write(ESP_LOG_INFO, tag, "I (%ld) %s: ", esp_log_timestamp(), tag);
    esp_log_write(ESP_LOG_INFO, tag, format, args...);
    esp_log_write(ESP_LOG_INFO, tag, "\n");
  }

  template <typename... Args>
  static void Error(StrideSubsystem sub, const char *format, Args... args)
  {
    const char *tag = get_tag(sub);
    esp_log_write(ESP_LOG_ERROR, tag, LOG_COLOR_E "E (%ld) %s: ", esp_log_timestamp(), tag);
    esp_log_write(ESP_LOG_ERROR, tag, format, args...);
    esp_log_write(ESP_LOG_ERROR, tag, LOG_RESET_COLOR "\n");
  }

  // template<typename... Args>
  // static void Warning(const char* format, Args... args);

private:
  StrideLogger() = default;
  static constexpr const char *get_tag(StrideSubsystem subsystem)
  {
    switch (subsystem)
    {
    case StrideSubsystem::Network:
      return "STRIDE_NET";
    case StrideSubsystem::Server:
      return "STRIDE_SRV";
    default:
      return "STRIDE";
    }
  }
};
