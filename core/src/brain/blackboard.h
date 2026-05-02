#pragma once
#include <cstdint>
#include <string>
#include <driver/gpio.h>

#include "network.h"
#include "server.h"

struct Blackboard
{
  Blackboard() = delete;

  // Network
  static inline std::string AccessSSID = "Stride";
  static inline std::string AccessPASS = "12345678";
  static inline int MaxNetConnections = 4;

  static inline bool IsNetConnected = false;
  static inline std::string IpAddress = "192.168.4.1";
  static inline int MaxConnectionRetries = 5;
  static inline NetworkMode NetworkMode = NetworkMode::Access;
  static inline gpio_num_t NetLed = GPIO_NUM_27;

  // Server
  static uint16_t inline Port = 80;
  static inline ServerMode ServerMode = ServerMode::User;

  // System

  // Defaults
  static void Reset()
  {
    IsNetConnected = false;
    IpAddress = "192.168.4.1";
    MaxConnectionRetries = 5;
    Port = 80;
  }
};
