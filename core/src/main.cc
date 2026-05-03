#include "stride_logger.hpp"
#include "network.hpp"
#include "server.hpp"

#include "server_mode_task.hpp"
#include "reload_task.hpp"
#include "card_task.hpp"

extern "C" void app_main(void)
{
  class Network network;
  network.connect();

  class Server server;
  server.start_server();

  StrideLed server_mode_led(GPIO_NUM_26, true);
  Blackboard::CurrentServerMode.subscribe([&server_mode_led](const auto &mode)
                                          {
    StrideLogger::Log(StrideSubsystem::Server, "Server changed mode");
    server_mode_led.toggle(); });

  xTaskCreatePinnedToCore(
      hear_server_mode_button,
      "HearServerModeButton",
      4096,
      NULL,
      5,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      hear_program_reload_button,
      "HearServerModeButton",
      4096,
      NULL,
      5,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      open_card,
      "OpenCard",
      4096,
      NULL,
      5,
      NULL,
      1);

  while (true)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
