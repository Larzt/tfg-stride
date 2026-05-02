#include "stride_logger.hpp"
#include "network.hpp"
#include "server.hpp"

extern "C" void app_main(void)
{
  class Network network;
  network.connect();

  class Server *server = new class Server();
  server->start_server();

  StrideLed mode_led(GPIO_NUM_26);
  mode_led.on();

  while (true)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
