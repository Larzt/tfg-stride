#pragma once
#include <cstdint>
#include <string>
#include <driver/gpio.h>

#include "types.hpp"

struct Blackboard
{
  Blackboard() = delete;

  // Network
  static inline std::string AccessSSID = "Stride";
  static inline std::string AccessPASS = "12345678";
  static inline int MaxNetConnections = 4;

  static inline bool IsNetConnected = false;
  static inline std::string WifiIpAddress = "0.0.0.0";
  static inline std::string LocalIpAddress = "0.0.0.0";
  static inline int MaxConnectionRetries = 5;
  static inline NetworkMode CurrentNetworkMode = NetworkMode::Access;
  static inline gpio_num_t NetLed = GPIO_NUM_27;

  // Server
  static uint16_t inline Port = 80;
  static uint16_t inline HeaderLength = 1024;
  static uint16_t inline MaxHandlers = 12;
  static inline ServerMode CurrentServerMode = ServerMode::Developer;

  // System

  // Handlers
  static inline gpio_num_t PingLed = GPIO_NUM_25;

  // Defaults
  static void Reset()
  {
    IsNetConnected = false;
    WifiIpAddress = "0.0.0.0";
    LocalIpAddress = "0.0.0.0";
    MaxConnectionRetries = 5;
    Port = 80;
  }
};
